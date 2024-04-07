// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MeleeStrike/MeleeStrike.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Characters/MLCharacterBase.h"
#include "AbilitySystem/MLAttributeSet.h"
#include "AbilitySystem/MLAbilityFunctions.h"
#include "AbilitySystem/MLAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"



void UMeleeStrike::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AMLCharacterBase* ActivatingCharacter = Cast<AMLCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!ActivatingCharacter) { return; }

	UMLAbilitySystemComponent* InstigatorASC = Cast<UMLAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (!InstigatorASC) { return; }

	

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		MLA_EndAbility(true);
		return;
	}

	InstigatorASC->AddReplicatedLooseGameplayTag(CanFeintTag);

	UAbilityTask_PlayMontageAndWait* MeleeStrikeMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(TEXT("StrikeMontageTask")), ActivatingCharacter->GetMontageByType(EMontageType::MeleeStrike));
	MeleeStrikeMontageTask->OnInterrupted.AddDynamic(this, &UMeleeStrike::StrikeInterrupt);
	MeleeStrikeMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* MeleeStrikeEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.MeleeStrike"))), nullptr, false, false);
	MeleeStrikeEventTask->EventReceived.AddDynamic(this, &UMeleeStrike::OnMeleeStrikeEventReceived);
	MeleeStrikeEventTask->ReadyForActivation();

}

void UMeleeStrike::StrikeInterrupt()
{
	MLA_EndAbility(true);
}

void UMeleeStrike::PerformStrikeLineTrace(FVector TraceStart, FVector TraceEnd)
{
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActors(AlreadyHitActors);
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(GetAvatarActorFromActorInfo());
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, LineTraceCollisionChannel, TraceParams, FCollisionResponseParams::DefaultResponseParam);
	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, true, 10.f, 0, 1.f);

	if (HitResult.bBlockingHit)
	{
		ApplyHit(HitResult);
	}
}

void UMeleeStrike::RecursiveStrikeTrace(FVector Location, FVector Nearest1, FVector Nearest2, const FVector& LastDirection)
{
	AMLCharacterBase* InstigatingCharacter = Cast<AMLCharacterBase>(GetAvatarActorFromActorInfo());
	if (!InstigatingCharacter) { return; }


	double DistanceFromRoot = FVector::Distance(InstigatingCharacter->GetWeaponRootLocation(), Location);

	FVector TraceStart = LastWeaponRootLocation + LastDirection * DistanceFromRoot;

	PerformStrikeLineTrace(TraceStart, Location);

	if (!FVector::PointsAreNear(Location, Nearest1, DistanceBetweenRecursiveTraces)) {
		FVector Midpoint = UMLAbilityFunctions::GetMidpoint(Location, Nearest1);
		RecursiveStrikeTrace(Midpoint, Nearest1, Location, LastDirection);
	}
	if (!FVector::PointsAreNear(Location, Nearest2, DistanceBetweenRecursiveTraces)) {
		FVector Midpoint = UMLAbilityFunctions::GetMidpoint(Location, Nearest2);
		RecursiveStrikeTrace(Midpoint, Nearest2, Location, LastDirection);
	}
}

void UMeleeStrike::BeginStrikeTraceForTick()
{
	AMLCharacterBase* InstigatingCharacter = Cast<AMLCharacterBase>(GetAvatarActorFromActorInfo());
	if (!InstigatingCharacter) { return; }

	if (IsFirstStrikeTrace)
	{
		LastWeaponRootLocation = InstigatingCharacter->GetWeaponRootLocation();
		LastWeaponTipLocation = InstigatingCharacter->GetWeaponTipLocation();
		IsFirstStrikeTrace = false;
		return;
	}

	PerformStrikeLineTrace(LastWeaponRootLocation, InstigatingCharacter->GetWeaponRootLocation());
	PerformStrikeLineTrace(LastWeaponTipLocation, InstigatingCharacter->GetWeaponTipLocation());
	FVector Midpoint = UMLAbilityFunctions::GetMidpoint(InstigatingCharacter->GetWeaponTipLocation(), InstigatingCharacter->GetWeaponRootLocation());
	FVector LastWeaponDirection = LastWeaponTipLocation - LastWeaponRootLocation;
	LastWeaponDirection.Normalize();
	RecursiveStrikeTrace(Midpoint, InstigatingCharacter->GetWeaponTipLocation(), InstigatingCharacter->GetWeaponRootLocation(), LastWeaponDirection);

	LastWeaponRootLocation = InstigatingCharacter->GetWeaponRootLocation();
	LastWeaponTipLocation = InstigatingCharacter->GetWeaponTipLocation();
}


void UMeleeStrike::ApplyHit(FHitResult& HitResult)
{
	IDamagable* HitActorAsDamagable = Cast<IDamagable>(HitResult.GetActor());
	if (!HitActorAsDamagable)
	{
		HitStop();
		return;
	}

	EBlockResult BlockResult = UMLAbilityFunctions::CheckBlocked(HitResult, GetAvatarActorFromActorInfo());
	bool EnemyHitStops = HitActorAsDamagable->DoesHitStop();
	Missed = false;
	UMLAbilitySystemComponent* InstigatorASC = Cast<UMLAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	InstigatorASC->RemoveReplicatedLooseGameplayTag(CannotCastTag);


	switch (BlockResult)
	{
		case EBlockResult::NotBlocked:
		{
			ApplyDamage(HitResult);
			if (EnemyHitStops)
			{
				HitStop();
			}
			break;
		}
		case EBlockResult::Blocked:
		{
			if (EnemyHitStops)
			{
				HitStop();
			}
			break;
		}
		case EBlockResult::Parried:
		{
			HitStop();
			break;
		}
	}

}

void UMeleeStrike::HitStop()
{
	UKismetSystemLibrary::PrintString(this, TEXT("HitStop"));
	AMLCharacterBase* InstigatingCharacter = Cast<AMLCharacterBase>(GetAvatarActorFromActorInfo());
	if (!InstigatingCharacter) { return; }

	InstigatingCharacter->Multicast_UseUpperBodyPoseSnapshotForDuration(0.2f);

	MLA_EndAbility(false);

}

void UMeleeStrike::ApplyDamage(FHitResult& HitResult)
{

	if (!MLA_HasAuthority()) { return; }

	FInstigatorAndTargetASInfo InstigatorAndTarget = GetInstigatorAndTargetASInfo(HitResult.GetActor());
	if (!InstigatorAndTarget.Success) { return; }

	InstigatorAndTarget.Instigator.ASC->ApplyGameplayEffectToTarget(StrikeHitEffect.GetDefaultObject(), InstigatorAndTarget.Target.ASC, 1.f);
	float HitImpulse = InstigatorAndTarget.Instigator.AttributeSet->GetHitImpulseMultiplier();
	InstigatorAndTarget.Target.Character->ApplyHitImpulseAndMontage(InstigatorAndTarget.Instigator.Character->GetActorLocation(), InstigatorAndTarget.Target.Character->GetActorLocation(), HitImpulse);

	AlreadyHitActors.Add(HitResult.GetActor());

}

void UMeleeStrike::OnMeleeStrikeEventReceived(FGameplayEventData Payload)
{
	if (!MLA_HasAuthority()) { return; }

	if (Payload.EventTag == StrikeStartEventTag)
	{
		UMLAbilitySystemComponent* InstigatorASC = Cast<UMLAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
		InstigatorASC->AddReplicatedLooseGameplayTag(CannotCastTag);
		InstigatorASC->RemoveReplicatedLooseGameplayTag(CanFeintTag);
		LineTraceActive = true;
		CanFeint = false;
	}
	else if (Payload.EventTag == StrikeEndEventTag)
	{
		MLA_EndAbility(false);
	}
}

void UMeleeStrike::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	LineTraceActive = false;
	
	if (MLA_HasAuthority())
	{
		UMLAbilitySystemComponent* InstigatorASC = Cast<UMLAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
		InstigatorASC->RemoveReplicatedLooseGameplayTag(CannotCastTag);
		InstigatorASC->RemoveReplicatedLooseGameplayTag(CanFeintTag);
		if (CanFeint)
		{
			CommitAbilityCooldown(Handle, ActorInfo, ActivationInfo, true);
		}
		else if (Missed)
		{
			UKismetSystemLibrary::PrintString(this, TEXT("Miss"));
			InstigatorASC->AddGameplayTagForDuration(CannotCastTag, 1.0f, true);
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

TStatId UMeleeStrike::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(MyTickableClass, STATGROUP_Tickables);
}

void UMeleeStrike::Tick(float DeltaTime)
{
	if (LineTraceActive && MLA_HasAuthority())
	{
		BeginStrikeTraceForTick();
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Kick/Kick.h"
#include "Characters/MLCharacterBase.h"
#include "AbilitySystem/MLAbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"



void UKick::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	FCharacterAbilitySystemInfo InstigatorInfo = GetInstigatorAbilitySystemInfo();
	if (!InstigatorInfo.Success) { return; }

	InstigatorInfo.ASC->ApplyGameplayEffectToSelf(WhileKickingEffect.GetDefaultObject(), 1.f, InstigatorInfo.ASC->MakeEffectContext());
	
	UAnimMontage* KickMontage = InstigatorInfo.Character->GetMontageByType(EMontageType::Kick);
	UAbilityTask_PlayMontageAndWait* KickMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(TEXT("KickMontageTask")), KickMontage);
	KickMontageTask->OnInterrupted.AddDynamic(this, &UKick::OnBlendOut);
	KickMontageTask->OnBlendOut.AddDynamic(this, &UKick::OnBlendOut);
	KickMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* EventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.Kick.Trigger"))), nullptr, false, false);
	EventTask->EventReceived.AddDynamic(this, &UKick::TriggerKick);
	EventTask->ReadyForActivation();


}

void UKick::OnBlendOut()
{
	MLA_EndAbility(false);
}

void UKick::OnInterrupted()
{
	MLA_EndAbility(true);
}

void UKick::TriggerKick(FGameplayEventData Payload)
{
	FCharacterAbilitySystemInfo InstigatorInfo = GetInstigatorAbilitySystemInfo();
	if (!InstigatorInfo.Success) { return; }

	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(GetAvatarActorFromActorInfo());

	FVector Start = InstigatorInfo.Character->GetKickFootLocation();
	FVector ScaledForwardVector = InstigatorInfo.Character->GetActorForwardVector() * (30.f);
	FVector End = Start + ScaledForwardVector;
	Start -= ScaledForwardVector;

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, LineTraceCollisionChannel, TraceParams, FCollisionResponseParams::DefaultResponseParam);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, 100.f, 0, 2.f);
}

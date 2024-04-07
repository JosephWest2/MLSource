// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Bow/ShootBow.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/MLProjectile.h"
#include "Kismet/KismetMathLibrary.h"



void UShootBow::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AMLCharacterBase* ActivatingCharacter = Cast<AMLCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!ActivatingCharacter) { return; }

	UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &UShootBow::OnBowRelease);
	WaitInputReleaseTask->ReadyForActivation();

	UAbilityTask_PlayMontageAndWait* PullBowMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(TEXT("PullBowMontageTask")), ActivatingCharacter->GetMontageByType(EMontageType::PullBow));
	PullBowMontageTask->OnBlendOut.AddDynamic(this, &UShootBow::OnBowPulled);
	PullBowMontageTask->ReadyForActivation();
}

void UShootBow::OnBowRelease(float TimeHeld)
{
	AMLCharacterBase* ActivatingCharacter = Cast<AMLCharacterBase>(GetAvatarActorFromActorInfo());
	if (!ActivatingCharacter) { return; }

	UAbilitySystemComponent* InstigatorASC = ActivatingCharacter->GetAbilitySystemComponent();
	if (!InstigatorASC) { return; }

	if (HoldingBowPulledEffectHandle.IsValid())
	{
		InstigatorASC->RemoveActiveGameplayEffect(HoldingBowPulledEffectHandle);
	}

	MontageStop();

	if (!ReadyToFire)
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
	}
	else
	{
		UAbilityTask_PlayMontageAndWait* ReleaseBowMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(TEXT("ReleaseBowMontageTask")), ActivatingCharacter->GetMontageByType(EMontageType::ReleaseBow));
		ReleaseBowMontageTask->ReadyForActivation();

		if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority)
		{
			SpawnArrow();

			CommitAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());
			EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
		}
	}
}

void UShootBow::OnBowPulled()
{
	AMLCharacterBase* ActivatingCharacter = Cast<AMLCharacterBase>(GetAvatarActorFromActorInfo());
	if (!ActivatingCharacter) { return; }

	UAbilityTask_PlayMontageAndWait* HoldBowPulledMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(TEXT("HoldBowPulledMontageTask")), ActivatingCharacter->GetMontageByType(EMontageType::HoldBowPulled));
	HoldBowPulledMontageTask->ReadyForActivation();

	UAbilitySystemComponent* InstigatorASC = ActivatingCharacter->GetAbilitySystemComponent();
	if (!InstigatorASC) { return; }

	HoldingBowPulledEffectHandle = InstigatorASC->ApplyGameplayEffectToSelf(HoldingBowPulledEffect.GetDefaultObject(), 1.0f, InstigatorASC->MakeEffectContext());

	ReadyToFire = true;
}

void UShootBow::SpawnArrow()
{
	AMLCharacterBase* ActivatingCharacter = Cast<AMLCharacterBase>(GetAvatarActorFromActorInfo());
	if (!ActivatingCharacter) { return; }

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
	SpawnInfo.Owner = SpawnInfo.Instigator->GetOwner();
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector Start = ActivatingCharacter->GetProjectileSpawnLocation();
	FVector End = Start + ActivatingCharacter->GetActorForwardVector() * 800;
	FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);

	GetWorld()->SpawnActor<AMLProjectile>(ProjectileClass, ActivatingCharacter->GetProjectileSpawnLocation(), SpawnRotation, SpawnInfo);	
}

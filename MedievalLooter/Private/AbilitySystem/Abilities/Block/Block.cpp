// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Block/Block.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"



void UBlock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AMLCharacterBase* ActivatingCharacter = Cast<AMLCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!ActivatingCharacter) { return; }

	UAbilitySystemComponent* InstigatorASC = ActivatingCharacter->GetAbilitySystemComponent();
	if (!InstigatorASC) { return; }

	ActivatingCharacter->BlockMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	FGameplayTag ParryTag = FGameplayTag::RequestGameplayTag(FName(TEXT("CharacterState.Blocking.Parrying")));
	InstigatorASC->AddLooseGameplayTag(ParryTag);

	GetWorld()->GetTimerManager().SetTimer(ParryTimerHandle, this, &UBlock::CloseParryWindow, 0.5f, false, 0.f);

	UAbilityTask_WaitInputRelease* WaitInputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
	WaitInputReleaseTask->OnRelease.AddDynamic(this, &UBlock::OnInputReleased);
	WaitInputReleaseTask->ReadyForActivation();

	UAnimMontage* BlockMontage = ActivatingCharacter->GetMontageByType(EMontageType::PriorityBlock);
	if (!BlockMontage)
	{
		BlockMontage = ActivatingCharacter->GetMontageByType(EMontageType::Block);
	}
	UAbilityTask_PlayMontageAndWait* BlockMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(TEXT("BlockMontageTask")), BlockMontage);
	BlockMontageTask->ReadyForActivation();

	UAbilityTask_WaitGameplayEvent* WaitHitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.Hit"))), nullptr, false, false);
	WaitHitEventTask->EventReceived.AddDynamic(this, &UBlock::OnHit);
	WaitHitEventTask->ReadyForActivation();

	InstigatorASC->ApplyGameplayEffectToSelf(WhileBlockingEffect.GetDefaultObject(), 1.0f, InstigatorASC->MakeEffectContext());
}

void UBlock::OnInputReleased(float TimeHeld)
{
	AMLCharacterBase* ActivatingCharacter = Cast<AMLCharacterBase>(GetAvatarActorFromActorInfo());
	if (!ActivatingCharacter) { return; }

	UAbilitySystemComponent* InstigatorASC = ActivatingCharacter->GetAbilitySystemComponent();
	if (!InstigatorASC) { return; }

	ActivatingCharacter->BlockMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InstigatorASC->RemoveActiveGameplayEffectBySourceEffect(WhileBlockingEffect, InstigatorASC);
	MontageStop();
	CommitAbilityCooldown(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UBlock::OnHit(FGameplayEventData Payload)
{
	AMLCharacterBase* ActivatingCharacter = Cast<AMLCharacterBase>(GetAvatarActorFromActorInfo());
	if (!ActivatingCharacter) { return; }
	
	if (CanParry)
	{
		DidParry = true;
	}
	else
	{
		CommitAbilityCost(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());
	}
}

void UBlock::CloseParryWindow()
{
	AMLCharacterBase* ActivatingCharacter = Cast<AMLCharacterBase>(GetAvatarActorFromActorInfo());
	if (!ActivatingCharacter) { return; }

	UAbilitySystemComponent* InstigatorASC = ActivatingCharacter->GetAbilitySystemComponent();
	if (!InstigatorASC) { return; }

	CanParry = false;
	FGameplayTag ParryTag = FGameplayTag::RequestGameplayTag(FName(TEXT("CharacterState.Blocking.Parrying")));
	InstigatorASC->RemoveLooseGameplayTag(ParryTag);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Jump/Jump.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PawnMovementComponent.h"



void UJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	FCharacterAbilitySystemInfo Instigator = GetInstigatorAbilitySystemInfo();
	if (!Instigator.Success) { return; }

	if (Instigator.Character->GetMovementComponent()->IsFalling()) { return; }

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		MLA_EndAbility(true);
		return;
	}

	Instigator.Character->Jump();
	JumpStarted = true;

}

void UJump::Tick(float DeltaTime)
{
	if (!JumpStarted) { return; }

	FCharacterAbilitySystemInfo Instigator = GetInstigatorAbilitySystemInfo();
	if (Instigator.Success && !Instigator.Character->GetMovementComponent()->IsFalling())
	{
		MLA_EndAbility(false);
	}
}

TStatId UJump::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(MyTickableClass, STATGROUP_Tickables);
}
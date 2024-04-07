// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/MLGameplayAbility.h"
#include "Jump.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALLOOTER_API UJump : public UMLGameplayAbility, public FTickableGameObject
{
	GENERATED_BODY()

private:

	bool JumpStarted = false;

public:

	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};

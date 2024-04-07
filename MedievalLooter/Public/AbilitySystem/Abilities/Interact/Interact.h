// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/MLGameplayAbility.h"
#include "Interact.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALLOOTER_API UInteract : public UMLGameplayAbility
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "ML | Interact")
	float InteractRange = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = "ML | Interact")
	float InteractSphereRadius = 100.f;

public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};

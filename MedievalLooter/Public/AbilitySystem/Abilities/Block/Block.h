// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/MLGameplayAbility.h"
#include "Block.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALLOOTER_API UBlock : public UMLGameplayAbility
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UGameplayEffect> WhileBlockingEffect;

	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	UFUNCTION()
	void OnHit(FGameplayEventData Payload);

	void CloseParryWindow();

	FTimerHandle ParryTimerHandle;

	bool CanParry = true;

	bool DidParry = false;
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};

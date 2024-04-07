// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/MLGameplayAbility.h"
#include "Kick.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALLOOTER_API UKick : public UMLGameplayAbility
{
	GENERATED_BODY()

private:

	UFUNCTION()
	void OnBlendOut();

	UFUNCTION()
	void OnInterrupted();

	UFUNCTION()
	void TriggerKick(FGameplayEventData Payload);

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UGameplayEffect> WhileKickingEffect;

	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> LineTraceCollisionChannel;
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};

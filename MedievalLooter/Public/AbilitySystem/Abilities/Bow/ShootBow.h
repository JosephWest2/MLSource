// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/MLGameplayAbility.h"
#include "ShootBow.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALLOOTER_API UShootBow : public UMLGameplayAbility
{
	GENERATED_BODY()

private:

	UFUNCTION()
	void OnBowRelease(float TimeHeld);

	UFUNCTION()
	void OnBowPulled();

protected:

	virtual void SpawnArrow();

	bool ReadyToFire = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UGameplayEffect> HoldingBowPulledEffect;

	FActiveGameplayEffectHandle HoldingBowPulledEffectHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class AMLProjectile> ProjectileClass;
	
public:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};

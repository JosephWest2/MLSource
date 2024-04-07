// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MLAbilitySystemComponent.generated.h"

/**
 * 
 */


UCLASS(meta = (BlueprintSpawnableComponent), Category = "ML | AbilitySystem")
class MEDIEVALLOOTER_API UMLAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

private:

	TMap<int, FTimerHandle> TagRemovalTimers;

	int TagRemovalTimerIndex = 0;

	void RemoveTimedTag(const FGameplayTag Tag, int HandleIndex, bool Replicates);

public:

	UFUNCTION(BlueprintCallable, Category = "ML | AbilitySystem")
	void CancelAbilitiesByTags(const TArray<FGameplayTag> WithTags, const TArray<FGameplayTag> WithoutTags, UGameplayAbility* Ignore);

	UFUNCTION(BlueprintCallable, Category = "ML | AbilitySystem")
	void AddGameplayTagForDuration(const FGameplayTag Tag, float Duration, bool Replicates);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ConsumableDataAsset.generated.h"

/**
 * 
 */


UCLASS(BlueprintType, Category = "Consumable")
class MEDIEVALLOOTER_API UConsumableDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
		TArray<TSubclassOf<class UGameplayEffect>> ConsumeEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
		TSubclassOf<class UAnimMontage> ConsumeAnimation;
};
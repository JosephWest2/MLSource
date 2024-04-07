// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MLAbilityFunctions.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EBlockResult : uint8 {
	NotBlocked,
	Blocked,
	Parried
};

UCLASS()
class MEDIEVALLOOTER_API UMLAbilityFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "ML | AbilityFunctions")
	static EBlockResult CheckBlocked(FHitResult HitResult, AActor* AttackingActor);

	UFUNCTION(BlueprintCallable, Category = "ML | AbilityFunctions")
	static void ApplyDamage(AActor* DamageDealer, AActor* DamageReceiver, TSubclassOf<UGameplayEffect> DamageEffect, FVector HitDirection);

	UFUNCTION(BlueprintCallable, Category = "ML | AbilityFunctions")
	static void ApplyEffects(AActor* EffectDealer, AActor* EffectReceiver, const TArray<TSubclassOf<UGameplayEffect>> Effects);

	static FVector GetMidpoint(const FVector& Vec1, const FVector& Vec2);
};

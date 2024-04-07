// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Characters/MLCharacter.h"
#include "MLGameplayAbility.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FCharacterAbilitySystemInfo {

	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AMLCharacterBase* Character;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMLAbilitySystemComponent* ASC;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMLAttributeSet* AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Success;
};

USTRUCT(BlueprintType)
struct FInstigatorAndTargetASInfo {

	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterAbilitySystemInfo Instigator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCharacterAbilitySystemInfo Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Success;
};

UCLASS(Blueprintable, Category = "ML | AbilitySystem")
class MEDIEVALLOOTER_API UMLGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable, Category = "ML | AbilitySystem")
	FCharacterAbilitySystemInfo GetInstigatorAbilitySystemInfo();

	UFUNCTION(BlueprintCallable, Category = "ML | AbilitySystem")
	FCharacterAbilitySystemInfo GetActorAbilitySystemInfo(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "ML | AbilitySystem")
	FInstigatorAndTargetASInfo GetInstigatorAndTargetASInfo(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "ML | AbilitySystem")
	bool MLA_HasAuthority();

	UFUNCTION(BlueprintCallable, Category = "ML | AbilitySystem")
	void MLA_EndAbility(bool WasCancelled);

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ML | AbilitySystem")
	EMLAbilityInputID InputID;

};

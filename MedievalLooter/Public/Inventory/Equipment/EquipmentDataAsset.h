// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "EquipmentDataAsset.generated.h"

/**
 * 
 */


UENUM(BlueprintType, Category = "Inventory | Equipment | Montages")
enum class EMontageType : uint8 {
	None,
	MeleeStrike,
	PullBow,
	HoldBowPulled,
	ReleaseBow,
	Block,
	Kick,
	PriorityBlock,
	HitBack,
	HitLeft,
	HitRight,
	HitForward,
	Jump
};

USTRUCT(BlueprintType, Category = "Inventory | Equipment | Montages")
struct FMontageInfo {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMontageType MontageType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* Montage;
};

UENUM(BlueprintType, Category = "Inventory | Equipment | Meshes")
enum class EMeshIdentifier : uint8 {

	None,
	Head,
	Torso,
	Legs,
	RightGauntlet,
	LeftGauntlet,
	BackRight,
	BackLeft,
	Waist,
	RightHand,
	LeftHand

};

USTRUCT(BlueprintType, Category = "Inventory | Equipment | Meshes")
struct FSkeletalMeshInfo {

	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMeshIdentifier MeshIdentifier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<class USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UAnimInstance> AnimInstanceClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer GameplayTags;

};

UENUM(BlueprintType, Category = "Inventory | Equipment")
enum class EEquipmentSlot : uint8 {

	None,
	Head,
	Torso,
	Legs,
	RightGauntlet,
	LeftGauntlet,
	Back,
	RightHand,
	LeftHand

};

UCLASS(BlueprintType, Category = "Inventory | Equipment")
class MEDIEVALLOOTER_API UEquipmentDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory | Equipment")
	EEquipmentSlot EquipmentSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory | Equipment")
	bool TwoHanded;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory | Equipment")
	TArray<TSubclassOf<class UMLGameplayAbility>> GrantedAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory | Equipment")
	TArray<TSubclassOf<class UGameplayEffect>> EquipEffects;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory | Equipment")
	TArray<FMontageInfo> Montages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory | Equipment")
	TArray<FSkeletalMeshInfo> SkeletalMeshes;
};
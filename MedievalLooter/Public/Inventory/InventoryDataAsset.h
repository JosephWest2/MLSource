// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InventoryDataAsset.generated.h"

/**
 * 
 */

UENUM(BlueprintType, Category = "Inventory")
enum class EItemType : uint8 {
	None,
	Miscellaneous,
	Equipment,
	Consumable,
};

USTRUCT(BlueprintType, Category = "Inventory")
struct FInventoryItemInfo {

	GENERATED_USTRUCT_BODY()

	FInventoryItemInfo() : DataAsset(TSoftObjectPtr<UInventoryDataAsset>()), Count(0), PercentDurability(100.f), IsEquipped(false) {}

	FInventoryItemInfo(TSoftObjectPtr<UInventoryDataAsset> inDataAsset, int inCount, float inPercentDurability, bool inIsEquipped) : DataAsset(inDataAsset), Count(inCount), PercentDurability(100.f), IsEquipped(inIsEquipped) {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TSoftObjectPtr<UInventoryDataAsset> DataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int Count;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	float PercentDurability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory | Equipment")
	bool IsEquipped;

};

UCLASS(BlueprintType, Category = "Inventory")
class MEDIEVALLOOTER_API UInventoryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSoftObjectPtr<UPrimaryDataAsset> ItemTypeData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	bool IsStackable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	EItemType ItemType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	float ItemWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSoftObjectPtr<UTexture2D> ItemImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSoftObjectPtr<UStaticMesh> StaticMesh;
};
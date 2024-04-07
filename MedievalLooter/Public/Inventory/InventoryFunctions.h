// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryDataAsset.h"
#include "Inventory/Equipment/EquipmentDataAsset.h"
#include "Inventory/Consumable/ConsumableDataAsset.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryFunctions.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALLOOTER_API UInventoryFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	static bool ItemsAreEqual(const FInventoryItemInfo Item1, const FInventoryItemInfo Item2);

	UFUNCTION(BlueprintCallable, Category = "Invetory")
	static UInventoryDataAsset* GetInventoryData(const FInventoryItemInfo ItemInfo);

	UFUNCTION(BlueprintCallable, Category = "Invetory | Equipment")
	static UEquipmentDataAsset* GetEquipmentData(const FInventoryItemInfo ItemInfo);

	UFUNCTION(BlueprintCallable, Category = "Invetory | Consumable")
	static UConsumableDataAsset* GetConsumableData(const FInventoryItemInfo ItemInfo);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/Equipment/EquipmentDataAsset.h"
#include "EquipmentComponent.generated.h"

/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEquippedItemsChangedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemEquippedDelegate, FInventoryItemInfo, ItemInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemUnequippedDelegate, FInventoryItemInfo, ItemInfo);
DECLARE_DELEGATE_OneParam(FSkeletalMeshSetter, FSkeletalMeshInfo);
DECLARE_DELEGATE_OneParam(FMontageSetter, FMontageInfo);

//Consumables
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemConsumedDelegate, FInventoryItemInfo, ItemInfo);

UCLASS(BlueprintType, Category = "ML | Inventory | Equipment")
class MEDIEVALLOOTER_API UEquipmentComponent : public UInventoryComponent
{
	GENERATED_BODY()

private:

	void UnequipOverlappingEquipment(const UEquipmentDataAsset* EquipmentData);

	void SetMeshes(const TArray<FSkeletalMeshInfo> Meshes);
	
	void ClearMeshes(const TArray<FSkeletalMeshInfo> Meshes);

	void SetMontages(const TArray<FMontageInfo> Montages);

	void ClearMontages(const TArray<FMontageInfo> Montages);

protected:

	// returns -1 if not found or not equipped
	int GetEquippedItemIndex(const FInventoryItemInfo ItemInfo) const;

	virtual bool TryRemoveItem(const FInventoryItemInfo ItemInfo) override;

	UFUNCTION()
	bool TryEquipItem(const FInventoryItemInfo ItemInfo);

	UFUNCTION()
	bool TryUnequipItem(const FInventoryItemInfo ItemInfo);

public:

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "ML | Inventory | Equipment")
	void Server_TryEquipItem(const FInventoryItemInfo ItemInfo);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "ML | Inventory | Equipment")
	void Server_TryUnequipItem(const FInventoryItemInfo ItemInfo);

	UPROPERTY(BlueprintAssignable, Category = "ML | Inventory | Equipment")
	FItemEquippedDelegate ItemEquippedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "ML | Inventory | Equipment")
	FItemUnequippedDelegate ItemUnequippedDelegate;

	FSkeletalMeshSetter EquipmentMeshSetter;

	FSkeletalMeshSetter EquipmentMeshClearer;

	FMontageSetter EquipmentMontageSetter;

	FMontageSetter EquipmentMontageClearer;

//Consumables
protected:

	bool TryUseItem(FInventoryItemInfo ItemInfo);

	bool TryConsumeItem(FInventoryItemInfo ItemInfo);

public:

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "ML | Inventory | Consumable")
	void Server_UseItem(FInventoryItemInfo ItemInfo);

	UPROPERTY(BlueprintAssignable, Category = "ML | Inventory | Consumable")
	FItemConsumedDelegate ItemConsumedDelegate;
	
};

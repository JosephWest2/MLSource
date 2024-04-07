// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryFunctions.h"

bool UInventoryFunctions::ItemsAreEqual(const FInventoryItemInfo Item1, const FInventoryItemInfo Item2)
{
	if (Item1.DataAsset == Item2.DataAsset && Item1.PercentDurability == Item2.PercentDurability)
	{
		return true;
	}
	return false;
}

UInventoryDataAsset* UInventoryFunctions::GetInventoryData(const FInventoryItemInfo ItemInfo)
{
	if (UInventoryDataAsset* ItemData = ItemInfo.DataAsset.LoadSynchronous())
	{
		return ItemData;
	}
	return nullptr;
}

UEquipmentDataAsset* UInventoryFunctions::GetEquipmentData(const FInventoryItemInfo ItemInfo)
{
	if (UInventoryDataAsset* ItemData = ItemInfo.DataAsset.LoadSynchronous())
	{
		if (UEquipmentDataAsset* EquipmentData = Cast<UEquipmentDataAsset>(ItemData->ItemTypeData.LoadSynchronous()))
		{
			return EquipmentData;
		}
	}
	return nullptr;
}

UConsumableDataAsset* UInventoryFunctions::GetConsumableData(const FInventoryItemInfo ItemInfo)
{
	if (UInventoryDataAsset* ItemData = ItemInfo.DataAsset.LoadSynchronous())
	{
		if (UConsumableDataAsset* ConsumableData = Cast<UConsumableDataAsset>(ItemData->ItemTypeData.LoadSynchronous()))
		{
			return ConsumableData;
		}
	}
	return nullptr;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Equipment/EquipmentComponent.h"
#include "Inventory/Equipment/EquipmentDataAsset.h"
#include "Inventory/InventoryFunctions.h"
#include "Net/UnrealNetwork.h"




void UEquipmentComponent::UnequipOverlappingEquipment(const UEquipmentDataAsset* EquipmentData)
{
	if (!EquipmentData) { return; }

	if (EquipmentData->EquipmentSlot == EEquipmentSlot::LeftHand || EquipmentData->EquipmentSlot == EEquipmentSlot::RightHand)
	{
		for (int i = 0; i < Inventory.Num(); i++)
		{
			if (Inventory[i].IsEquipped)
			{
				const UEquipmentDataAsset* CurrentlyEquipped = UInventoryFunctions::GetEquipmentData(Inventory[i]);
				if (!CurrentlyEquipped) { return; }

				if (CurrentlyEquipped->EquipmentSlot == EquipmentData->EquipmentSlot || CurrentlyEquipped->TwoHanded || EquipmentData->TwoHanded)
				{
					TryUnequipItem(Inventory[i]);
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < Inventory.Num(); i++)
		{
			if (Inventory[i].IsEquipped)
			{
				const UEquipmentDataAsset* CurrentlyEquipped = UInventoryFunctions::GetEquipmentData(Inventory[i]);
				if (!CurrentlyEquipped) { return; }

				if (CurrentlyEquipped->EquipmentSlot == EquipmentData->EquipmentSlot)
				{
					TryUnequipItem(Inventory[i]);
				}
			}
		}
	}
}

void UEquipmentComponent::SetMeshes(const TArray<FSkeletalMeshInfo> Meshes)
{
	for (const FSkeletalMeshInfo& Mesh : Meshes)
	{
		EquipmentMeshSetter.ExecuteIfBound(Mesh);
	}
}

void UEquipmentComponent::ClearMeshes(const TArray<FSkeletalMeshInfo> Meshes)
{
	for (const FSkeletalMeshInfo& Mesh : Meshes)
	{
		EquipmentMeshClearer.ExecuteIfBound(Mesh);
	}
}

void UEquipmentComponent::SetMontages(const TArray<FMontageInfo> Montages)
{
	for (const FMontageInfo& Montage : Montages)
	{
		EquipmentMontageSetter.ExecuteIfBound(Montage);
	}
}

void UEquipmentComponent::ClearMontages(const TArray<FMontageInfo> Montages)
{
	for (const FMontageInfo& Montage : Montages)
	{
		EquipmentMontageClearer.ExecuteIfBound(Montage);
	}
}

int UEquipmentComponent::GetEquippedItemIndex(const FInventoryItemInfo ItemInfo) const
{
	int i = GetItemIndex(ItemInfo);
	if (i >= 0 && Inventory[i].IsEquipped)
	{
		return i;
	}
	return -1;
}

bool UEquipmentComponent::TryRemoveItem(const FInventoryItemInfo ItemInfo)
{
	TryUnequipItem(ItemInfo);
	return Super::TryAddItem(ItemInfo);
}

bool UEquipmentComponent::TryEquipItem(const FInventoryItemInfo ItemInfo)
{
	if (UEquipmentDataAsset* EquipmentData = UInventoryFunctions::GetEquipmentData(ItemInfo))
	{
		int i = GetItemIndex(ItemInfo);

		if (i >= 0)
		{
			
			UnequipOverlappingEquipment(EquipmentData);
			SetMeshes(EquipmentData->SkeletalMeshes);
			SetMontages(EquipmentData->Montages);
			Inventory[i].IsEquipped = true;
			ItemEquippedDelegate.Broadcast(ItemInfo);
			return true;
		}
	}
	return false;
}

bool UEquipmentComponent::TryUnequipItem(const FInventoryItemInfo ItemInfo)
{
	UEquipmentDataAsset* EquipmentData = UInventoryFunctions::GetEquipmentData(ItemInfo);
	if (!EquipmentData) { return false; }

	int i = GetItemIndex(ItemInfo);
	if (i >= 0)
	{
		ClearMeshes(EquipmentData->SkeletalMeshes);
		ClearMontages(EquipmentData->Montages);
		Inventory[i].IsEquipped = false;
		ItemUnequippedDelegate.Broadcast(ItemInfo);
		return true;
	}
	return false;
	
}

void UEquipmentComponent::Server_TryEquipItem_Implementation(const FInventoryItemInfo ItemInfo)
{
    TryEquipItem(ItemInfo);
}

void UEquipmentComponent::Server_TryUnequipItem_Implementation(const FInventoryItemInfo ItemInfo)
{
	TryUnequipItem(ItemInfo);
}

//Consumables

void UEquipmentComponent::Server_UseItem_Implementation(FInventoryItemInfo ItemInfo)
{
	TryUseItem(ItemInfo);
}

bool UEquipmentComponent::TryUseItem(FInventoryItemInfo ItemInfo)
{
	if (UInventoryDataAsset* DataAsset = UInventoryFunctions::GetInventoryData(ItemInfo))
	{
		switch (DataAsset->ItemType)
		{
		case EItemType::Consumable:
		{
			return TryConsumeItem(ItemInfo);
			break;
		}
		case EItemType::Equipment:
		{
			if (GetEquippedItemIndex(ItemInfo) >= 0)
			{
				return TryUnequipItem(ItemInfo);
			}
			else
			{
				return TryEquipItem(ItemInfo);
			}
			break;
		}

		}
	}
	return false;
}

bool UEquipmentComponent::TryConsumeItem(FInventoryItemInfo ItemInfo)
{
	if (UConsumableDataAsset* ConsumableData = UInventoryFunctions::GetConsumableData(ItemInfo))
	{
		ItemConsumedDelegate.Broadcast(ItemInfo);
		return true;
	}
	return false;
}
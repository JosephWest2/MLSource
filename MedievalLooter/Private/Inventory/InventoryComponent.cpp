// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/Pickup.h"
#include "Inventory/InventoryFunctions.h"
#include "Engine/AssetManager.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
	MaxWeight = 100.f;
	CurrentWeight = 0.f;

}

const TArray<FInventoryItemInfo> UInventoryComponent::GetInventory() const
{
	return Inventory;
}

float UInventoryComponent::GetMaxWeight() const
{
	return MaxWeight;
}

float UInventoryComponent::GetCurrentWeight() const
{
	return CurrentWeight;
}

void UInventoryComponent::Server_TryAddItem_Implementation(const FInventoryItemInfo ItemInfo)
{
	TryAddItem(ItemInfo);
}

void UInventoryComponent::Server_TryRemoveItem_Implementation(const FInventoryItemInfo ItemInfo)
{
	TryRemoveItem(ItemInfo);
}

void UInventoryComponent::Server_SetMaxWeight_Implementation(float NewMaxWeight)
{
	MaxWeight = NewMaxWeight;
}

void UInventoryComponent::Server_GiveItem_Implementation(const FInventoryItemInfo ItemInfo, UInventoryComponent* receivingComponent)
{
	if (UInventoryDataAsset* ItemData = ItemInfo.DataAsset.LoadSynchronous())
	{
		int i = GetItemIndex(ItemInfo);

		if (i != -1 && receivingComponent->CanAddItemWeight(ItemInfo) && TryRemoveItem(ItemInfo))
		{
			receivingComponent->TryAddItem(ItemInfo);
		}
	}
}

bool UInventoryComponent::ContainsItem(const FInventoryItemInfo ItemInfo) const
{
	for (auto& item : Inventory)
	{
		if (UInventoryFunctions::ItemsAreEqual(ItemInfo, item))
		{
			return true;
		}
	}
	return false;
}


void UInventoryComponent::OnRep_Inventory()
{
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		InventoryChangedDelegate.Broadcast();
	}
	
}

bool UInventoryComponent::CanAddItemWeight(const FInventoryItemInfo ItemInfo) const
{
	if (UInventoryDataAsset* ItemData = ItemInfo.DataAsset.LoadSynchronous())
	{
		if (ItemData->ItemWeight * ItemInfo.Count + CurrentWeight <= MaxWeight)
		{
			return true;
		}
	}
	return false;
}


int UInventoryComponent::GetItemIndex(const FInventoryItemInfo ItemInfo) const
{
	for (int i = 0; i < Inventory.Num(); i++)
	{
		if (UInventoryFunctions::ItemsAreEqual(ItemInfo, Inventory[i]))
		{
			return i;
		}
	}
	return -1;
}

bool UInventoryComponent::TryAddItem(const FInventoryItemInfo ItemInfo)
{

	if (UInventoryDataAsset* ItemData = ItemInfo.DataAsset.LoadSynchronous())
	{
		if (CanAddItemWeight(ItemInfo))
		{
			int i = GetItemIndex(ItemInfo);

			if (i != -1)
			{
				Inventory[i].Count += ItemInfo.Count;
			}
			else
			{
				Inventory.Emplace(ItemInfo);
			}
			CurrentWeight += ItemInfo.Count * ItemData->ItemWeight;
			AddItemDelegate.Broadcast(ItemInfo);
			return true;
		}
	}
	return false;
}

bool UInventoryComponent::TryRemoveItem(const FInventoryItemInfo ItemInfo)
{
	if (UInventoryDataAsset* ItemData = ItemInfo.DataAsset.LoadSynchronous())
	{
		int i = GetItemIndex(ItemInfo);

		if (i != -1)
		{
			FInventoryItemInfo& item = Inventory[i];

			if (item.Count == ItemInfo.Count)
			{
				Inventory.RemoveAt(i);
			}
			else
			{
				item.Count -= ItemInfo.Count;
			}
			CurrentWeight -= ItemInfo.Count * ItemData->ItemWeight;
			RemoveItemDelegate.Broadcast(ItemInfo);
			return true;
		}
	}
	return false;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, Inventory);
	DOREPLIFETIME(UInventoryComponent, MaxWeight);
	DOREPLIFETIME(UInventoryComponent, CurrentWeight);
}

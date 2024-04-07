// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryDataAsset.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryChangedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemRemovedDelegate, FInventoryItemInfo, ItemInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemAddedDelegate, FInventoryItemInfo, ItemInfo);

UCLASS(BlueprintType, Category = "ML | Inventory")
class MEDIEVALLOOTER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()


private:

	UPROPERTY(Replicated, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float MaxWeight;

	UPROPERTY(Replicated)
	float CurrentWeight;

	UFUNCTION()
	void OnRep_Inventory();

protected:

	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	TArray<FInventoryItemInfo> Inventory;

	// Returns -1 if not found
	int GetItemIndex(const FInventoryItemInfo ItemInfo) const;

	virtual bool TryRemoveItem(const FInventoryItemInfo ItemInfo);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	

	UInventoryComponent();

	virtual bool TryAddItem(const FInventoryItemInfo ItemInfo);

	UFUNCTION(BlueprintCallable, Category = "ML | Inventory")
	bool CanAddItemWeight(const FInventoryItemInfo ItemInfo) const;

	UFUNCTION(BlueprintCallable, Category = "ML | Inventory")
	const TArray<FInventoryItemInfo> GetInventory() const;

	UFUNCTION(BlueprintCallable, Category = "ML | Inventory")
	float GetMaxWeight() const;

	UFUNCTION(BlueprintCallable, Category = "ML | Inventory")
	float GetCurrentWeight() const;

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "ML | Inventory")
	void Server_TryAddItem(const FInventoryItemInfo ItemInfo);
	
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "ML | Inventory")
	void Server_TryRemoveItem(const FInventoryItemInfo ItemInfo);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "ML | Inventory")
	void Server_SetMaxWeight(float NewMaxWeight);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "ML | Inventory")
	void Server_GiveItem(const FInventoryItemInfo ItemInfo, UInventoryComponent* receivingComponent);

	UFUNCTION(BlueprintCallable, Category = "ML | Inventory")
	bool ContainsItem(const FInventoryItemInfo ItemInfo) const;

	UPROPERTY(BlueprintAssignable, Category = "ML | Inventory")
	FInventoryChangedDelegate InventoryChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "ML | Inventory")
	FItemRemovedDelegate RemoveItemDelegate;

	UPROPERTY(BlueprintAssignable, Category = "ML | Inventory")
	FItemAddedDelegate AddItemDelegate;


};

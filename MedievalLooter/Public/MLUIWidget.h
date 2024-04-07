// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryDataAsset.h"
#include "MLUIWidget.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALLOOTER_API UMLUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetHealth(float NewHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetMaxHealth(float NewMaxHealth);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetStamina(float NewStamina);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetMaxStamina(float NewMaxStamina);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetInventory(UPARAM(ref) const TArray<FInventoryItemInfo>& NewInventory);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetEquipment(UPARAM(ref) const TArray<FInventoryItemInfo>& NewEquipment);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ToggleInventory(bool ShowInventory);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ToggleMap(bool ShowMap);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ToggleMenu(bool ShowMenu);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void SetInteractableText(UPARAM(ref) const FText& InteractableText);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ShowCrosshair(bool ShowCrosshair);
};

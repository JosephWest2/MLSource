// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "InventoryDataAsset.h"
#include "Pickup.generated.h"

UCLASS(BlueprintType)
class MEDIEVALLOOTER_API APickup : public AActor, public IInteractable
{
	GENERATED_BODY()

protected:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
	TObjectPtr<class UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(ReplicatedUsing = OnRep_ItemInfo, EditInstanceOnly, BlueprintReadWrite, Category = "Pickup", meta = (ExposeOnSpawn = "true"))
	FInventoryItemInfo ItemInfo;

	UFUNCTION()
	void OnRep_ItemInfo();

public:	
	
	APickup();

	virtual FText GetInteractionText() const;

	virtual void OnInteract(class AMLCharacter* InteractingCharacter);

};

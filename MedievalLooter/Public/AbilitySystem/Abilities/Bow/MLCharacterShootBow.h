// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Bow/ShootBow.h"
#include "MLCharacterShootBow.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALLOOTER_API UMLCharacterShootBow : public UShootBow
{
	GENERATED_BODY()
	
protected:

	virtual void SpawnArrow() override;
};

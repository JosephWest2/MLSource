// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayTagContainer.h"
#include "TaggableSkeletalMeshComponent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Category = "Mesh")
class MEDIEVALLOOTER_API UTaggableSkeletalMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:

	FGameplayTagContainer GameplayTags = FGameplayTagContainer();
	
};

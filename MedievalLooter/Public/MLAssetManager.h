// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MLAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALLOOTER_API UMLAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:

	static UMLAssetManager& Get();

	/** Starts initial load, gets called from InitializeObjectReferences */
	virtual void StartInitialLoading() override;
};

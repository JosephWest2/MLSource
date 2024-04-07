// Fill out your copyright notice in the Description page of Project Settings.


#include "MLAssetManager.h"
#include "AbilitySystemGlobals.h"


UMLAssetManager& UMLAssetManager::Get()
{
	UMLAssetManager* Singleton = Cast<UMLAssetManager>(GEngine->AssetManager);

	if (Singleton)
	{
		return *Singleton;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be MLAssetManager!"));
		return *NewObject<UMLAssetManager>();	 // never calls this
	}
}


void UMLAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	UAbilitySystemGlobals::Get().InitGlobalData();
}
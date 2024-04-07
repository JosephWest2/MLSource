// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MLPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALLOOTER_API AMLPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "MLUI")
	TSubclassOf<class UMLUIWidget> UIWidgetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "MLUI")
	TObjectPtr<class UMLUIWidget> UIWidget;

public:

	void CreateUI();

	UFUNCTION(BlueprintCallable, Category = "MLUI")
	class UMLUIWidget* GetUIWidget();
	
};

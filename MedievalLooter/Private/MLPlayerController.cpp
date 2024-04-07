// Fill out your copyright notice in the Description page of Project Settings.


#include "MLPlayerController.h"
#include "Characters/MLCharacter.h"
#include "MLUIWidget.h"
#include "AbilitySystem/MLAttributeSet.h"

void AMLPlayerController::CreateUI()
{
	if (UIWidget || !UIWidgetClass || !IsLocalPlayerController())
	{
		return;
	}
	if (AMLCharacter* Char = Cast<AMLCharacter>(GetPawn()))
	{
		UIWidget = CreateWidget<UMLUIWidget>(this, UIWidgetClass);
		UIWidget->AddToViewport();

		if (UMLAttributeSet* AttributeSet = Char->GetAttributeSet())
		{
			UIWidget->SetHealth(AttributeSet->GetHealth());
			UIWidget->SetMaxHealth(AttributeSet->GetMaxHealth());
			UIWidget->SetStamina(AttributeSet->GetStamina());
			UIWidget->SetMaxStamina(AttributeSet->GetMaxStamina());
		}
	}

}

UMLUIWidget* AMLPlayerController::GetUIWidget()
{
	return UIWidget;
}

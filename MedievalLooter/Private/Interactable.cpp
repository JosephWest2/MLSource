// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable.h"

// Add default functionality here for any IInteractable functions that are not pure virtual.

FText IInteractable::GetInteractionText() const
{
    return FText::FromString("Default Interaction Text");
}

void IInteractable::OnInteract(AMLCharacter* InteractingCharacter)
{
    return;
}

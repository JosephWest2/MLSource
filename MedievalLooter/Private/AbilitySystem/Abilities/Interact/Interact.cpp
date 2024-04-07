// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Interact/Interact.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interactable.h"

void UInteract::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AMLCharacter* Instigator = Cast<AMLCharacter>(ActorInfo->OwnerActor);
	if (!Instigator) { return; }

	FVector Start = Instigator->GetActorLocation();
	FVector Range = FVector(InteractRange, InteractRange, InteractRange);
	FVector End = Instigator->GetFollowCamera()->GetForwardVector() * Range + Start;

	FHitResult OutHit;
	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_Visibility);

	TArray<AActor*> OutActors;
	TArray<AActor*> ActorsToIgnore;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), OutHit.Location, InteractSphereRadius, ObjectTypes, nullptr, ActorsToIgnore, OutActors);

	for (AActor* Actor : OutActors)
	{
		if (IInteractable* Interactable = Cast<IInteractable>(Actor))
		{
			Interactable->OnInteract(Instigator);
			break;
		}
	}

	MLA_EndAbility(false);
}

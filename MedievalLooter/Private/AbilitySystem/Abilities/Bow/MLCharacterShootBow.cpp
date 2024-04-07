// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Bow/MLCharacterShootBow.h"
#include "Characters/MLCharacter.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"


void UMLCharacterShootBow::SpawnArrow()
{
	AMLCharacter* ActivatingCharacter = Cast<AMLCharacter>(GetAvatarActorFromActorInfo());
	if (!ActivatingCharacter) { return; }

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Instigator = Cast<APawn>(GetAvatarActorFromActorInfo());
	SpawnInfo.Owner = SpawnInfo.Instigator->GetOwner();
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector Start = ActivatingCharacter->GetProjectileSpawnLocation();
	FVector End = ActivatingCharacter->GetFollowCamera()->GetComponentLocation() + ActivatingCharacter->GetFollowCamera()->GetForwardVector() * 1200;
	FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(Start, End);

	GetWorld()->SpawnActor<AMLProjectile>(ProjectileClass, Start, SpawnRotation, SpawnInfo);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MLProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"


AMLProjectile::AMLProjectile()
{

	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComponent"));
	SphereCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMLProjectile::OnProjectileOverlap);

}


void AMLProjectile::BeginPlay()
{
	Super::BeginPlay();
}


void AMLProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Internal_OnProjectileOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	OnProjectileOverlapDelegate.Broadcast(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AMLProjectile::Internal_OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}


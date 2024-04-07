// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "MLProjectile.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnProjectileOverlap, UPrimitiveComponent*, OverlappedComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, int32, OtherBodyIndex, bool, bFromSweep, const FHitResult&, SweepResult);

UCLASS()
class MEDIEVALLOOTER_API AMLProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	
	AMLProjectile();

protected:
	
	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ML | Projectile")
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ML | Projectile")
	USphereComponent* SphereCollisionComponent;

	virtual void Internal_OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(BlueprintAssignable, Category = "ML | Projectile")
	FOnProjectileOverlap OnProjectileOverlapDelegate;
};

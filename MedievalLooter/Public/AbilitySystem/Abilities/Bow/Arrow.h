// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MLProjectile.h"
#include "Arrow.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALLOOTER_API AArrow : public AMLProjectile
{
	GENERATED_BODY()
public:

	AArrow();

private:

	bool CanDamage = true;

	UFUNCTION()
	void OnProjectileStop(const FHitResult& ImpactResult);

	void AttachArrowToHitComponent(FHitResult& HitResult);

	void TryArrowStick(UPrimitiveComponent* HitComponent);

protected:

	virtual void Internal_OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Arrow")
	TEnumAsByte<ECollisionChannel> ArrowTraceCollisionChannel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Arrow")
	TSubclassOf<class UGameplayEffect> ArrowDamageEffect;

};

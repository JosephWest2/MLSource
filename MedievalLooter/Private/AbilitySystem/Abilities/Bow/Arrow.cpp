// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Bow/Arrow.h"
#include "Characters/MLCharacterBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/MLAttributeSet.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystem/MLAbilityFunctions.h"

AArrow::AArrow()
{
	ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AArrow::OnProjectileStop);
}

void AArrow::OnProjectileStop(const FHitResult& ImpactResult)
{
	CanDamage = false;
}

void AArrow::Internal_OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetInstigator()) { return; }

	AMLCharacterBase* HitCharacter = Cast<AMLCharacterBase>(OtherActor);
	if (!HitCharacter || !CanDamage || HitCharacter->GetMesh() != OtherComp || GetVelocity().Length() < 3000.f)
	{
		CanDamage = false;
		return;
	}
	CanDamage = false;
	
	
	if (HasAuthority())
	{
		UMLAbilityFunctions::ApplyDamage(GetInstigator(), OtherActor, ArrowDamageEffect, GetActorForwardVector());
	}

	TryArrowStick(OtherComp);

}

void AArrow::TryArrowStick(UPrimitiveComponent* HitComponent)
{
	FVector SphereLocation = SphereCollisionComponent->GetComponentLocation();
	FVector SphereDirection = SphereCollisionComponent->GetForwardVector();
	FVector TraceStart = SphereLocation - SphereDirection * 100;
	FVector TraceEnd = SphereLocation + SphereDirection * 100;
	FCollisionQueryParams LineParams(SCENE_QUERY_STAT(KismetTraceComponent), true);

	FHitResult HitResult;
	bool DidHit = HitComponent->LineTraceComponent(HitResult, TraceStart, TraceEnd, LineParams);

	if (DidHit && abs(FVector::DotProduct(SphereDirection, HitResult.Normal)) > 0.5 && HitResult.BoneName != FName(TEXT("")))
	{
		AttachArrowToHitComponent(HitResult);
	}
}

void AArrow::AttachArrowToHitComponent(FHitResult& HitResult)
{
	SetActorEnableCollision(false);
	ProjectileMovementComponent->StopSimulating(HitResult);
	FAttachmentTransformRules AttachmetRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	AttachToComponent(HitResult.GetComponent(), AttachmetRules, HitResult.BoneName);
	SetActorLocation(HitResult.Location + GetActorForwardVector() * 10);
}

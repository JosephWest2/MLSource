// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MLAbilityFunctions.h"
#include "Characters/MLCharacterBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/MLAttributeSet.h"

EBlockResult UMLAbilityFunctions::CheckBlocked(FHitResult HitResult, AActor* AttackingActor)
{
	AMLCharacterBase* AttackingCharacter = Cast<AMLCharacterBase>(AttackingActor);
	AMLCharacterBase* DefendingCharacter = Cast<AMLCharacterBase>(HitResult.GetActor());
	if (!AttackingCharacter || !DefendingCharacter)
	{
		return EBlockResult::NotBlocked;
	}

	FGameplayTag BlockTag = FGameplayTag::RequestGameplayTag(FName(TEXT("CharacterState.Blocking")));
	if (!DefendingCharacter->GetAbilitySystemComponent()->HasMatchingGameplayTag(BlockTag))
	{
		return EBlockResult::NotBlocked;
	}

	FVector DefendingLocation = DefendingCharacter->GetActorLocation();
	FVector HitLocation = HitResult.Location;
	FVector OutwardHitDirection = HitLocation - DefendingLocation;

	FVector DefendingForward = DefendingCharacter->GetActorForwardVector();
	FRotator DefendingRotation = DefendingCharacter->GetMesh()->GetSocketRotation(FName(TEXT("BlockMeshSocket")));
	FVector2D SphericalForward = DefendingForward.UnitCartesianToSpherical();

	double Pitch = DefendingRotation.Pitch > 180.f ? DefendingRotation.Pitch - 360.f : DefendingRotation.Pitch;
	Pitch += 90.f;
	Pitch = 180 - Pitch;
	double PitchRadians = UKismetMathLibrary::DegreesToRadians(Pitch);

	SphericalForward.X = PitchRadians;

	FVector DefendingLookDirection = SphericalForward.SphericalToUnitCartesian();

	DefendingLookDirection.Normalize();
	OutwardHitDirection.Normalize();
	double DotProduct = FVector::DotProduct(DefendingLookDirection, OutwardHitDirection);

	if (DotProduct <= 0.2)
	{
		return EBlockResult::NotBlocked;
	}
	
	FGameplayTag ParryTag = FGameplayTag::RequestGameplayTag(FName(TEXT("CharacterState.Blocking.Parrying")));
	if (DefendingCharacter->GetAbilitySystemComponent()->HasMatchingGameplayTag(ParryTag))
	{
		return EBlockResult::Parried;
	}
	return EBlockResult::Blocked;
}

void UMLAbilityFunctions::ApplyDamage(AActor* DamageDealer, AActor* DamageReceiver, TSubclassOf<UGameplayEffect> DamageEffect, FVector HitDirection)
{
	AMLCharacterBase* HitCharacter = Cast<AMLCharacterBase>(DamageReceiver);
	if (!HitCharacter) { return; }

	AMLCharacterBase* InstigatingCharacter = Cast<AMLCharacterBase>(DamageDealer);
	if (!InstigatingCharacter) { return; }

	UAbilitySystemComponent* InstigatorASC = InstigatingCharacter->GetAbilitySystemComponent();
	if (!InstigatorASC) { return; }

	UAbilitySystemComponent* HitASC = HitCharacter->GetAbilitySystemComponent();
	if (!HitASC) { return; }

	UMLAttributeSet* InstigatorAttributes = InstigatingCharacter->GetAttributeSet();
	if (!InstigatorAttributes) { return; }

	InstigatorASC->ApplyGameplayEffectToTarget(DamageEffect.GetDefaultObject(), HitASC, 1.0f, InstigatorASC->MakeEffectContext());
	float HitImpulse = InstigatorAttributes->GetHitImpulseMultiplier();
	HitCharacter->ApplyHitImpulseAndMontageByDirection(HitDirection, HitImpulse);
}

void UMLAbilityFunctions::ApplyEffects(AActor* EffectDealer, AActor* EffectReceiver, const TArray<TSubclassOf<UGameplayEffect>> Effects)
{
	AMLCharacterBase* HitCharacter = Cast<AMLCharacterBase>(EffectReceiver);
	if (!HitCharacter) { return; }

	AMLCharacterBase* InstigatingCharacter = Cast<AMLCharacterBase>(EffectDealer);
	if (!InstigatingCharacter) { return; }

	UAbilitySystemComponent* InstigatorASC = InstigatingCharacter->GetAbilitySystemComponent();
	if (!InstigatorASC) { return; }

	UAbilitySystemComponent* HitASC = HitCharacter->GetAbilitySystemComponent();
	if (!HitASC) { return; }

	for (TSubclassOf<UGameplayEffect> Effect : Effects)
	{
		InstigatorASC->ApplyGameplayEffectToTarget(Effect.GetDefaultObject(), HitASC, 1.0f, InstigatorASC->MakeEffectContext());
	}
}

FVector UMLAbilityFunctions::GetMidpoint(const FVector& Vec1, const FVector& Vec2)
{
	return FVector((Vec1.X + Vec2.X)/2.0, (Vec1.Y + Vec2.Y)/2.0, (Vec1.Z + Vec2.Z)/2.0);
}

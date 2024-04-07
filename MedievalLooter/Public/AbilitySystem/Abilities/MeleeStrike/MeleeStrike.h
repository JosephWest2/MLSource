// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/MLGameplayAbility.h"
#include "MeleeStrike.generated.h"

/**
 * 
 */
UCLASS()
class MEDIEVALLOOTER_API UMeleeStrike : public UMLGameplayAbility, public FTickableGameObject
{
	GENERATED_BODY()
	
private:

	UFUNCTION()
	void StrikeInterrupt();

	void ApplyHit(FHitResult& HitResult);

	void PerformStrikeLineTrace(FVector TraceStart, FVector TraceEnd);

	void RecursiveStrikeTrace(FVector Location, FVector Nearest1, FVector Nearest2, const FVector& LastDirection);
	
	void BeginStrikeTraceForTick();

	TArray<AActor*> AlreadyHitActors;

	UFUNCTION()
	void OnMeleeStrikeEventReceived(FGameplayEventData Payload);

	FTimerHandle LineTraceTimerHandle;

	void HitStop();

	void ApplyDamage(FHitResult& HitResult);

	bool Missed = true;

	bool LineTraceActive = false;

	bool CanFeint = true;

	bool IsFirstStrikeTrace = true;

	FVector LastWeaponRootLocation;

	FVector LastWeaponTipLocation;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class UGameplayEffect> StrikeHitEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TEnumAsByte<ECollisionChannel> LineTraceCollisionChannel;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag CannotCastTag = FGameplayTag::RequestGameplayTag(FName(TEXT("CharacterState.CannotCast")));

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag StrikeStartEventTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.MeleeStrike.Event.Start")));

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag StrikeEndEventTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.MeleeStrike.Event.End")));

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag CanFeintTag = FGameplayTag::RequestGameplayTag(FName(TEXT("Ability.MeleeStrike.CanFeint")));

	UPROPERTY(EditDefaultsOnly)
	double DistanceBetweenRecursiveTraces = 5.f;

public:

	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};

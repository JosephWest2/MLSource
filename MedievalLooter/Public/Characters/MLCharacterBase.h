// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Inventory/InventoryInterface.h"
#include "Inventory/InventoryDataAsset.h"
#include "Inventory/Equipment/EquipmentDataAsset.h"
#include "Damagable.h"
#include "MLCharacterBase.generated.h"



UENUM(BlueprintType)
enum class EMontageSlot : uint8 {
	UpperBody,
	FullBody
};


UCLASS()
class MEDIEVALLOOTER_API AMLCharacterBase : public ACharacter, public IAbilitySystemInterface, public IInventoryInterface, public IDamagable
{

	GENERATED_BODY()

private:

	UFUNCTION()
	void InitializeAttributes();

protected:

	virtual void SetMesh(FSkeletalMeshInfo MeshInfo);

	virtual void ClearMesh(FSkeletalMeshInfo MeshInfo);

	virtual void SetMontage(FMontageInfo MontageInfo);

	virtual void ClearMontage(FMontageInfo MontageInfo);

	void InitializeDefaultMontages();

	UFUNCTION()
	void ApplyItemEquipEffects(FInventoryItemInfo ItemInfo);

	UFUNCTION()
	void RemoveItemEquipEffects(FInventoryItemInfo ItemInfo);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ML | AbilitySystem")
	TObjectPtr<class UMLAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "ML | InventorySystem")
	TObjectPtr<class UEquipmentComponent> EquipmentComponent;

	UPROPERTY()
	TObjectPtr<class UMLAttributeSet> AttributeSet;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay();

	FDelegateHandle HealthChangedDeathDelegateHandle;

	FDelegateHandle MovementSpeedChangedDelegateHandle;

	UPROPERTY(ReplicatedUsing = OnRep_IsDead, EditDefaultsOnly, BlueprintReadWrite, Category = "ML | Death")
	bool IsDead;

	UFUNCTION()
	void OnRep_IsDead();

	UFUNCTION(BlueprintCallable, Category = "ML | Death")
	void Die();

	bool AttributesInitialized = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ML | HitStop")
	bool HitStops;

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ML | AbilitySystem")
	TArray<TSubclassOf<class UGameplayEffect>> DefaultGameplayEffects;

	UFUNCTION()
	void OnRep_ReplicatedSkeletalMeshInfo();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ML | Meshes")
	TMap<EMeshIdentifier, class UTaggableSkeletalMeshComponent*> SkeletalMeshComponentReplicationRegister;

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedSkeletalMeshInfo, BlueprintReadWrite, Category = "ML | Meshes")
	TArray<FSkeletalMeshInfo> ReplicatedSkeletalMeshInfo;

	UPROPERTY(Replicated)
	TArray<FMontageInfo> Montages;

	UPROPERTY(EditDefaultsOnly, Category = "ML | Montages")
	TMap<EMontageType, class UAnimMontage*> DefaultMontages;

	UFUNCTION(BlueprintCallable, Category = "ML | Animation")
	void PlayHitMontage(FVector HitDirection);

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable, Category = "ML | Animation")
	void Multicast_PlayHitMontage(FVector HitDirection);

	UFUNCTION(BlueprintCallable, Category = "ML")
	void ApplyHitImpulseByDirection(FVector HitDirection, float ImpulseMultiplier);

	UFUNCTION(BlueprintCallable, Category ="ML")
	void ApplyHitImpulse(FVector LocationFrom, FVector LocationTo, float ImpulseMultiplier);

	UFUNCTION(NetMulticast, Unreliable, BlueprintCallable)
	void Multicast_ApplyMeshImpulse(FVector Impulse);

	UPROPERTY(EditDefaultsOnly, Category = "ML | Meshes")
	TArray<FSkeletalMeshInfo> DefaultSkeletalMeshes;

	UPROPERTY(EditDefaultsOnly, Category = "ML | AbilitySystem")
	TArray<TSubclassOf<class UMLGameplayAbility>> DefaultGameplayAbilities;

public:

	AMLCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual bool DoesHitStop() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ML | Inventory")
	class UInventoryComponent* GetInventoryComponent() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "ML | Block")
	class UStaticMeshComponent* BlockMesh;
	
	UFUNCTION(BlueprintCallable)
	void ApplyHitImpulseAndMontageByDirection(FVector HitDirection, float ImpulseMultiplier);

	UFUNCTION(BlueprintCallable)
	void ApplyHitImpulseAndMontage(FVector LocationFrom, FVector LocationTo, float ImpulseMultiplier);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ML | Combat | MeleeStrike")
	FVector GetWeaponRootLocation() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ML | Combat | MeleeStrike")
	FVector GetWeaponTipLocation() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ML | Combat | Bow")
	FVector GetBowPullingHandLocation() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ML | Combat | Projectile")
	FVector GetProjectileSpawnLocation() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ML | Combat | Kick")
	FVector GetKickFootLocation() const;

	UPROPERTY(BlueprintReadWrite, Category = "ML | Combat")
	bool BowIsPulled = false;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ML | Animation")
	void UseUpperBodyPoseSnapshotForDuration(float Duration);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "ML | Animation")
	void Multicast_UseUpperBodyPoseSnapshotForDuration(float Duration);

	UFUNCTION(BlueprintCallable, Category = "ML | AbilitySystem")
	UMLAttributeSet* GetAttributeSet() const;

	UFUNCTION(BlueprintCallable, Category = "ML | Animation")
	class UAnimMontage* GetMontageByType(EMontageType MontageType) const;

};

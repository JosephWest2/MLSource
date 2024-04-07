// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/MLCharacterBase.h"
#include "InputActionValue.h"
#include "Components/SkeletalMeshComponent.h"
#include "Inventory/InventoryInterface.h"
#include "Damagable.h"
#include "GameplayAbilitySpecHandle.h"
#include "Templates/Tuple.h"
#include "Inventory/InventoryDataAsset.h"
#include "AbilitySystemInterface.h"
#include "MLCharacter.generated.h"


UENUM(BlueprintType, Category = "ML | AbilitySystem")
enum class EMLAbilityInputID : uint8 {
	None,
	Confirm,
	Cancel,
	AttackOne,
	AttackTwo,
	Kick,
	AltActionOne,
	AltActionTwo,
	SpellOne,
	SpellTwo,
	Interact,
	Jump
};

USTRUCT()
struct FCurrentAbilityPair {
	GENERATED_USTRUCT_BODY()

	FCurrentAbilityPair()
	{
		AbilityClass = nullptr;
		AbilitySpecHandle = FGameplayAbilitySpecHandle();
	}

	FCurrentAbilityPair(TSubclassOf<class UMLGameplayAbility> InClass, FGameplayAbilitySpecHandle InSpecHandle)
	{
		AbilityClass = InClass;
		AbilitySpecHandle = InSpecHandle;
	}

	UPROPERTY()
	TSubclassOf<class UMLGameplayAbility> AbilityClass;

	UPROPERTY()
	FGameplayAbilitySpecHandle AbilitySpecHandle;
};

UCLASS(config=Game)
class AMLCharacter : public AMLCharacterBase
{

	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* InventoryMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* MenuMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* MapMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* OpenInventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CloseInventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* OpenMenuAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CloseMenuAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* OpenMapAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* CloseMapAction;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTaggableSkeletalMeshComponent> HeadMesh;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTaggableSkeletalMeshComponent> LegsMesh;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTaggableSkeletalMeshComponent> TorsoMesh;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTaggableSkeletalMeshComponent> RightHandMesh;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTaggableSkeletalMeshComponent> LeftHandMesh;

	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UTaggableSkeletalMeshComponent> BackMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ML | AbilitySystem", meta = (AllowPrivateAccess = "true"))
	TMap<EMLAbilityInputID, class UInputAction*> InputActionAbilityBindings;

	UFUNCTION()
	void SetupAbilitySystemInputBindings();

	UFUNCTION()
	void SendInputToAbilitySystemComponent(bool isPressed, EMLAbilityInputID inputID);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ML | AbilitySystem", meta = (AllowPrivateAccess = "true"))
	TMap<EMLAbilityInputID, TSubclassOf<class UMLGameplayAbility>> DefaultAbilities;

	UFUNCTION()
	void GrantDefaultAbilities();

	UPROPERTY()
	TMap<EMLAbilityInputID, FCurrentAbilityPair> CurrentAbilites;

	UPROPERTY()
	FText InteractableText;

	UFUNCTION()
	void UpdateInteractableUI();

	UFUNCTION()
	void UpdateInteractable();

	UFUNCTION()
	void ToggleCrouching(bool ShouldCrouch);

	UFUNCTION()
	void OnInventoryChanged();


protected:

	FTimerHandle InteractableTimerHandle;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void ToggleInventory(bool ShowInventory);

	void ToggleMap(bool ShowMap);

	void ToggleMenu(bool ShowMenu);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void BeginPlay();

	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable, Category = "ML | AbilitySystem")
	void GrantAbility(UPARAM(ref)const TSubclassOf<class UMLGameplayAbility>& NewAbility);

	UFUNCTION(BlueprintCallable, Category = "ML | AbilitySystem")
	void RemoveAbility(UPARAM(ref)const TSubclassOf<class UMLGameplayAbility>& OutAbility);

	UFUNCTION(BlueprintCallable, Category = "ML | AbilitySystem")
	void GrantAbilities(UPARAM(ref)const TArray<TSubclassOf<class UMLGameplayAbility>>& NewAbilities);

	UFUNCTION(BlueprintCallable, Category = "ML | AbilitySystem")
	void RemoveAbilities(UPARAM(ref)const TArray<TSubclassOf<class UMLGameplayAbility>>& OutAbilities);

	FDelegateHandle HealthChangedUIDelegateHandle;
	FDelegateHandle MaxHealthChangedUIDelegateHandle;
	FDelegateHandle StaminaChangedUIDelegateHandle;
	FDelegateHandle MaxStaminaChangedUIDelegateHandle;

	UFUNCTION()
	void GrantEquipmentAbilities(FInventoryItemInfo ItemInfo);

	UFUNCTION()
	void RemoveEquipmentAbilities(FInventoryItemInfo ItemInfo);


public:

	AMLCharacter();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE class UMLAttributeSet* GetAttributeSet() const { return AttributeSet; }

};


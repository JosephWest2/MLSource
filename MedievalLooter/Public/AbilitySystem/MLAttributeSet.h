// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MLAttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS(Category = "ML | AbilitySystem")
class MEDIEVALLOOTER_API UMLAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:

	UMLAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "ML | Attribute", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UMLAttributeSet, Health)
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UPROPERTY(BlueprintReadOnly, Category = "ML | Attribute", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMLAttributeSet, MaxHealth)
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UPROPERTY(BlueprintReadOnly, Category = "ML | Attribute", ReplicatedUsing = OnRep_Stamina)
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UMLAttributeSet, Stamina)
	UFUNCTION()
	virtual void OnRep_Stamina(const FGameplayAttributeData& OldStamina);

	UPROPERTY(BlueprintReadOnly, Category = "ML | Attribute", ReplicatedUsing = OnRep_MaxStamina)
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UMLAttributeSet, MaxStamina)
	UFUNCTION()
	virtual void OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina);

	UPROPERTY(BlueprintReadOnly, Category = "ML | Attribute", ReplicatedUsing = OnRep_MeleeDamage)
	FGameplayAttributeData MeleeDamage;
	ATTRIBUTE_ACCESSORS(UMLAttributeSet, MeleeDamage)
	UFUNCTION()
	virtual void OnRep_MeleeDamage(const FGameplayAttributeData& OldMeleeDamage);

	UPROPERTY(BlueprintReadOnly, Category = "ML | Attribute", ReplicatedUsing = OnRep_RangedDamage)
	FGameplayAttributeData RangedDamage;
	ATTRIBUTE_ACCESSORS(UMLAttributeSet, RangedDamage)
	UFUNCTION()
	virtual void OnRep_RangedDamage(const FGameplayAttributeData& OldRangedDamage);

	UPROPERTY(BlueprintReadOnly, Category = "ML | Attribute", ReplicatedUsing = OnRep_HitImpulseMultiplier)
	FGameplayAttributeData HitImpulseMultiplier;
	ATTRIBUTE_ACCESSORS(UMLAttributeSet, HitImpulseMultiplier)
	UFUNCTION()
	virtual void OnRep_HitImpulseMultiplier(const FGameplayAttributeData& OldHitImpulseMultiplier);

	UPROPERTY(BlueprintReadOnly, Category = "ML | Attribute", ReplicatedUsing = OnRep_DoesWeaponHitStop)
	FGameplayAttributeData DoesWeaponHitStop;
	ATTRIBUTE_ACCESSORS(UMLAttributeSet, DoesWeaponHitStop)
	UFUNCTION()
	virtual void OnRep_DoesWeaponHitStop(const FGameplayAttributeData& OldDoesWeaponHitStop);

	UPROPERTY(BlueprintReadOnly, Category = "ML | Attribute", ReplicatedUsing = OnRep_MovementSpeed)
	FGameplayAttributeData MovementSpeed;
	ATTRIBUTE_ACCESSORS(UMLAttributeSet, MovementSpeed)
	UFUNCTION()
	virtual void OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed);
};

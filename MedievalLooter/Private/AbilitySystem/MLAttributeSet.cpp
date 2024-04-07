// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/MLAttributeSet.h"
#include "Net/UnrealNetwork.h"

UMLAttributeSet::UMLAttributeSet()
{
}

void UMLAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		SetHealth(FMath::Min<float>(NewValue, GetHealth()));
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		SetStamina(FMath::Min<float>(NewValue, GetStamina()));
	}
	else if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Min<float>(NewValue, GetMaxHealth());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Min<float>(NewValue, GetMaxStamina());
	}
	

}

void UMLAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UMLAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMLAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMLAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMLAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMLAttributeSet, MeleeDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMLAttributeSet, RangedDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMLAttributeSet, HitImpulseMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMLAttributeSet, DoesWeaponHitStop, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMLAttributeSet, MovementSpeed, COND_None, REPNOTIFY_Always);

}

void UMLAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMLAttributeSet, Health, OldHealth);
}

void UMLAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMLAttributeSet, MaxHealth, OldMaxHealth);
}

void UMLAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMLAttributeSet, Stamina, OldStamina);
}

void UMLAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMLAttributeSet, MaxStamina, OldMaxStamina);
}

void UMLAttributeSet::OnRep_MeleeDamage(const FGameplayAttributeData& OldMeleeDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMLAttributeSet, MeleeDamage, OldMeleeDamage);
}

void UMLAttributeSet::OnRep_RangedDamage(const FGameplayAttributeData& OldRangedDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMLAttributeSet, RangedDamage, OldRangedDamage);
}

void UMLAttributeSet::OnRep_HitImpulseMultiplier(const FGameplayAttributeData& OldHitImpulseMultiplier)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMLAttributeSet, HitImpulseMultiplier, OldHitImpulseMultiplier);
}

void UMLAttributeSet::OnRep_DoesWeaponHitStop(const FGameplayAttributeData& OldDoesWeaponHitStop)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMLAttributeSet, DoesWeaponHitStop, OldDoesWeaponHitStop);
}

void UMLAttributeSet::OnRep_MovementSpeed(const FGameplayAttributeData& OldMovementSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMLAttributeSet, MovementSpeed, OldMovementSpeed);
}

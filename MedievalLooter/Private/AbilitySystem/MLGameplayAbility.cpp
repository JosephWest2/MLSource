// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MLGameplayAbility.h"
#include "Characters/MLCharacterBase.h"
#include "AbilitySystem/MLAbilitySystemComponent.h"
#include "AbilitySystem/MLAttributeSet.h"

FCharacterAbilitySystemInfo UMLGameplayAbility::GetInstigatorAbilitySystemInfo()
{
	FCharacterAbilitySystemInfo Output =  FCharacterAbilitySystemInfo();

	Output.Character = Cast<AMLCharacterBase>(GetAvatarActorFromActorInfo());

	Output.ASC = Cast<UMLAbilitySystemComponent>(Output.Character->GetAbilitySystemComponent());

	Output.AttributeSet = Output.Character->GetAttributeSet();

	if (!Output.Character || !Output.ASC || !Output.AttributeSet)
	{
		Output.Success = false;
	}
	else
	{
		Output.Success = true;
	}
	return Output;
}

FCharacterAbilitySystemInfo UMLGameplayAbility::GetActorAbilitySystemInfo(AActor* Actor)
{
	FCharacterAbilitySystemInfo Output = FCharacterAbilitySystemInfo();

	Output.Character = Cast<AMLCharacterBase>(Actor);

	Output.ASC = Cast<UMLAbilitySystemComponent>(Output.Character->GetAbilitySystemComponent());

	Output.AttributeSet = Output.Character->GetAttributeSet();

	if (!Output.Character || !Output.ASC || !Output.AttributeSet)
	{
		Output.Success = false;
	}
	else
	{
		Output.Success = true;
	}
	return Output;
}

FInstigatorAndTargetASInfo UMLGameplayAbility::GetInstigatorAndTargetASInfo(AActor* Target)
{
	FInstigatorAndTargetASInfo Output = FInstigatorAndTargetASInfo();
	Output.Instigator = GetInstigatorAbilitySystemInfo();
	Output.Target = GetActorAbilitySystemInfo(Target);
	Output.Success = Output.Instigator.Success && Output.Target.Success;
	return Output;
}

bool UMLGameplayAbility::MLA_HasAuthority()
{
	return GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority;
}

void UMLGameplayAbility::MLA_EndAbility(bool WasCancelled)
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, WasCancelled);
}

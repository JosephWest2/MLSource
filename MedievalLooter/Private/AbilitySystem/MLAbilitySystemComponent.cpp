// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/MLAbilitySystemComponent.h"
#include "Kismet/KismetSystemLibrary.h"


void UMLAbilitySystemComponent::CancelAbilitiesByTags(const TArray<FGameplayTag> WithTags, const TArray<FGameplayTag> WithoutTags, UGameplayAbility* Ignore)
{
	FGameplayTagContainer WithTagsContainer = FGameplayTagContainer();
	FGameplayTagContainer WithoutTagsContainer = FGameplayTagContainer();
	for (const FGameplayTag& WithTag : WithTags)
	{
		WithTagsContainer.AddTag(WithTag);
	}
	for (const FGameplayTag& WithoutTag : WithoutTags)
	{
		WithoutTagsContainer.AddTag(WithoutTag);
	}
	CancelAbilities(&WithTagsContainer, &WithoutTagsContainer, Ignore);
}

void UMLAbilitySystemComponent::AddGameplayTagForDuration(const FGameplayTag Tag, float Duration, bool Replicates)
{
	if (Replicates)
	{
		AddReplicatedLooseGameplayTag(Tag);
	}
	else
	{
		AddLooseGameplayTag(Tag);
	}
	FTimerHandle NewHandle;
	FTimerDelegate Delegate;
	Delegate.BindUObject(this, &UMLAbilitySystemComponent::RemoveTimedTag, Tag, TagRemovalTimerIndex, Replicates);
	GetWorld()->GetTimerManager().SetTimer(NewHandle, Delegate, Duration, false);
	TagRemovalTimers.Add(TagRemovalTimerIndex, NewHandle);
	TagRemovalTimerIndex++;
}

void UMLAbilitySystemComponent::RemoveTimedTag(const FGameplayTag Tag, int HandleIndex, bool Replicates)
{
	if (Replicates)
	{
		RemoveReplicatedLooseGameplayTag(Tag);
	}
	else
	{
		RemoveLooseGameplayTag(Tag);
	}
	TagRemovalTimers.Remove(HandleIndex);
}

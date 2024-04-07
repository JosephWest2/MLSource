// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MLCharacterBase.h"
#include "AbilitySystem/MLAbilitySystemComponent.h"
#include "Inventory/Equipment/EquipmentComponent.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystem/MLAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Inventory/InventoryFunctions.h"
#include "AbilitySystem/MLGameplayAbility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TaggableSkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"



UAnimMontage* AMLCharacterBase::GetMontageByType(EMontageType MontageType) const
{
	for (const FMontageInfo& Montage : Montages)
	{
		if (Montage.MontageType == MontageType)
		{
			return Montage.Montage;
		}
	}
	return nullptr;
}

AMLCharacterBase::AMLCharacterBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UMLAbilitySystemComponent>("MLAbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UMLAttributeSet>("MLAttributeSet");

	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>("MLEquipmentComponent");
	EquipmentComponent->SetIsReplicated(true);

	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>("BlockMesh");
	BlockMesh->SetupAttachment(GetMesh(), FName(TEXT("BlockMeshSocket")));

}

void AMLCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMLCharacterBase, EquipmentComponent);
	DOREPLIFETIME(AMLCharacterBase, IsDead);
	DOREPLIFETIME(AMLCharacterBase, ReplicatedSkeletalMeshInfo);
	DOREPLIFETIME(AMLCharacterBase, Montages);
}

void AMLCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAttributes();

	if (AbilitySystemComponent && HasAuthority())
	{
		for (auto& Ability : DefaultGameplayAbilities)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, Ability.GetDefaultObject()->GetAbilityLevel(), -1, this);
			AbilitySystemComponent->GiveAbility(AbilitySpec);
		}
	}

	if (HasAuthority())
	{
		InitializeDefaultMontages();
	}
}

void AMLCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAttributes();
}

void AMLCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		HealthChangedDeathDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data) {

			if (HasAuthority() && AttributeSet->GetHealth() <= 0 && AttributesInitialized)
			{
				IsDead = true;
				Die();
			}

		});
		MovementSpeedChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMovementSpeedAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data) {

			GetCharacterMovement()->MaxWalkSpeed = AttributeSet->GetMovementSpeed();
			GetCharacterMovement()->MaxWalkSpeedCrouched = AttributeSet->GetMovementSpeed() / 2;

		});
	}

	if (EquipmentComponent)
	{
		EquipmentComponent->EquipmentMeshSetter.BindUObject(this, &AMLCharacterBase::SetMesh);
		EquipmentComponent->EquipmentMeshClearer.BindUObject(this, &AMLCharacterBase::ClearMesh);
		EquipmentComponent->EquipmentMontageSetter.BindUObject(this, &AMLCharacterBase::SetMontage);
		EquipmentComponent->EquipmentMontageClearer.BindUObject(this, &AMLCharacterBase::ClearMontage);
		EquipmentComponent->ItemEquippedDelegate.AddDynamic(this, &AMLCharacterBase::ApplyItemEquipEffects);
		EquipmentComponent->ItemUnequippedDelegate.AddDynamic(this, &AMLCharacterBase::ApplyItemEquipEffects);
	}
}

void AMLCharacterBase::InitializeAttributes()
{
	if (AbilitySystemComponent)
	{
		for (auto& Effect : DefaultGameplayEffects)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			AbilitySystemComponent->ApplyGameplayEffectToSelf(Effect.GetDefaultObject(), 1.f, EffectContext);
		}
	}
	AttributesInitialized = true;
}

void AMLCharacterBase::SetMesh(FSkeletalMeshInfo MeshInfo)
{
	bool Found = false;

	for (int i = 0; i < ReplicatedSkeletalMeshInfo.Num(); i++)
	{
		if (ReplicatedSkeletalMeshInfo[i].MeshIdentifier == MeshInfo.MeshIdentifier)
		{
			ReplicatedSkeletalMeshInfo[i] = MeshInfo;
			Found = true;
		}
	}

	if (!Found)
	{
		ReplicatedSkeletalMeshInfo.Add(MeshInfo);
	}
	OnRep_ReplicatedSkeletalMeshInfo();
}

void AMLCharacterBase::ClearMesh(FSkeletalMeshInfo MeshInfo)
{
	for (int i = 0; i < ReplicatedSkeletalMeshInfo.Num(); i++)
	{
		if (ReplicatedSkeletalMeshInfo[i].MeshIdentifier == MeshInfo.MeshIdentifier)
		{
			ReplicatedSkeletalMeshInfo.RemoveAt(i);
		}
	}
	OnRep_ReplicatedSkeletalMeshInfo();
}

void AMLCharacterBase::SetMontage(FMontageInfo MontageInfo)
{
	bool Found = false;

	for (int i = 0; i < Montages.Num(); i++)
	{
		if (Montages[i].MontageType == MontageInfo.MontageType)
		{
			Montages[i] = MontageInfo;
			Found = true;
		}
	}

	if (!Found)
	{
		Montages.Add(MontageInfo);
	}
}

void AMLCharacterBase::ClearMontage(FMontageInfo MontageInfo)
{
	for (int i = 0; i < Montages.Num(); i++)
	{
		FMontageInfo CurrentMontage = Montages[i];

		if (CurrentMontage.MontageType == MontageInfo.MontageType)
		{
			Montages.RemoveAt(i);
		}
		if (DefaultMontages.Contains(CurrentMontage.MontageType))
		{
			FMontageInfo DefaultMontage = FMontageInfo();
			DefaultMontage.Montage = DefaultMontages.FindRef(CurrentMontage.MontageType);
			DefaultMontage.MontageType = CurrentMontage.MontageType;

			Montages.Add(DefaultMontage);
		}
	}
}

void AMLCharacterBase::InitializeDefaultMontages()
{
	for (auto& MontagePair : DefaultMontages)
	{
		FMontageInfo MontageInfo = FMontageInfo();
		MontageInfo.Montage = MontagePair.Value;
		MontageInfo.MontageType = MontagePair.Key;

		Montages.Add(MontageInfo);
	}
}

void AMLCharacterBase::ApplyItemEquipEffects(FInventoryItemInfo ItemInfo)
{
	UEquipmentDataAsset* EquipmentData = UInventoryFunctions::GetEquipmentData(ItemInfo);
	if (!EquipmentData) { return; }

	for (const TSubclassOf<UGameplayEffect>& EquipEffect : EquipmentData->EquipEffects)
	{
		AbilitySystemComponent->ApplyGameplayEffectToSelf(EquipEffect.GetDefaultObject(), 1.0f, AbilitySystemComponent->MakeEffectContext());
	}
}

void AMLCharacterBase::RemoveItemEquipEffects(FInventoryItemInfo ItemInfo)
{
	UEquipmentDataAsset* EquipmentData = UInventoryFunctions::GetEquipmentData(ItemInfo);
	if (!EquipmentData) { return; }

	for (const TSubclassOf<UGameplayEffect>& EquipEffect : EquipmentData->EquipEffects)
	{
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(EquipEffect, AbilitySystemComponent);
	}
}

void AMLCharacterBase::OnRep_IsDead()
{
	if (IsDead)
	{
		Die();
	}
}

void AMLCharacterBase::Die()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->bPauseAnims = true;
	GetMovementComponent()->Deactivate();
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
		AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("CharacterState.Dead")));
	}
	if (AController* PC = Cast<AController>(GetOwner()))
	{
		PC->UnPossess();
	}
}

void AMLCharacterBase::OnRep_ReplicatedSkeletalMeshInfo()
{
	TArray<EMeshIdentifier> UnfoundMeshes;
	SkeletalMeshComponentReplicationRegister.GenerateKeyArray(UnfoundMeshes);

	for (FSkeletalMeshInfo& MeshInfo : ReplicatedSkeletalMeshInfo)
	{
		if (SkeletalMeshComponentReplicationRegister.Contains(MeshInfo.MeshIdentifier))
		{
			UnfoundMeshes.Remove(MeshInfo.MeshIdentifier);
			UTaggableSkeletalMeshComponent* MeshComponent = SkeletalMeshComponentReplicationRegister.FindRef(MeshInfo.MeshIdentifier);
			MeshComponent->SetSkeletalMeshAsset(MeshInfo.SkeletalMesh.LoadSynchronous());
			MeshComponent->SetAnimInstanceClass(MeshInfo.AnimInstanceClass);
			MeshComponent->GameplayTags.Reset();
			
			TArray<FGameplayTag> TagsToAdd = MeshInfo.GameplayTags.GetGameplayTagArray();
			for (const FGameplayTag& Tag : TagsToAdd)
			{
				MeshComponent->GameplayTags.AddTag(Tag);
			}
		}
	}
	for (EMeshIdentifier MeshIdentifier : UnfoundMeshes)
	{
		UTaggableSkeletalMeshComponent* MeshComponent = SkeletalMeshComponentReplicationRegister.FindRef(MeshIdentifier);

		bool Found = false;
		for (const FSkeletalMeshInfo& MeshInfo : DefaultSkeletalMeshes)
		{
			if (MeshInfo.MeshIdentifier == MeshIdentifier)
			{
				Found = true;
				MeshComponent->SetSkeletalMeshAsset(MeshInfo.SkeletalMesh.LoadSynchronous());
				MeshComponent->SetAnimInstanceClass(MeshInfo.AnimInstanceClass);
				MeshComponent->GameplayTags.Reset();

				TArray<FGameplayTag> TagsToAdd = MeshInfo.GameplayTags.GetGameplayTagArray();
				for (const FGameplayTag& Tag : TagsToAdd)
				{
					MeshComponent->GameplayTags.AddTag(Tag);
				}
			}
		}
		if (!Found)
		{
			MeshComponent->SetSkeletalMeshAsset(nullptr);
			MeshComponent->SetAnimInstanceClass(nullptr);
			MeshComponent->GameplayTags.Reset();
		}
	}
}

void AMLCharacterBase::Multicast_ApplyMeshImpulse_Implementation(FVector Impulse)
{
	GetMesh()->SetPhysicsLinearVelocity(Impulse, true);
}

UAbilitySystemComponent* AMLCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void AMLCharacterBase::PlayHitMontage(FVector HitDirection)
{
	FVector CharacterDirection = GetActorForwardVector();
	FVector2D CharacterDirectionSpherical = CharacterDirection.GetSafeNormal().UnitCartesianToSpherical();
	float CharacterPhi = CharacterDirectionSpherical.Y + PI;
	FVector2D HitDirectionSpherical = HitDirection.GetSafeNormal().UnitCartesianToSpherical();
	float HitPhi = HitDirectionSpherical.Y + PI;
	float difference = HitPhi - CharacterPhi;
	if (difference < 0)
	{
		difference += 2 * PI;
	}
	UKismetSystemLibrary::PrintString(this, "CharacterPhi: " + FString::SanitizeFloat(CharacterPhi));
	UKismetSystemLibrary::PrintString(this, "HitPhi: " + FString::SanitizeFloat(HitPhi));
	UKismetSystemLibrary::PrintString(this, "Difference: " + FString::SanitizeFloat(difference));

	difference += PI / 4;
	EMontageType MontageType;
	if (difference <= 0 && difference > PI / 2)
	{
		MontageType = EMontageType::HitForward;
	}
	else if (difference <= PI / 2 && difference > PI)
	{
		MontageType = EMontageType::HitLeft;
	}
	else if (difference <= PI && difference > 3 * PI / 2)
	{
		MontageType = EMontageType::HitBack;
	}
	else if (difference <= 3 * PI / 2 && difference > 2 * PI)
	{
		MontageType = EMontageType::HitRight;
	}
	else
	{
		MontageType = EMontageType::HitBack;
	}

	if (UAnimMontage* HitMontage = GetMontageByType(MontageType))
	{
		UKismetSystemLibrary::PrintString(this, "Montage Found");
		PlayAnimMontage(HitMontage);
	}
}

void AMLCharacterBase::ApplyHitImpulseByDirection(FVector HitDirection, float ImpulseMultiplier)
{
	UKismetMathLibrary::Vector_Normalize(HitDirection);
	HitDirection *= 50 * ImpulseMultiplier;
	GetCharacterMovement()->AddImpulse(HitDirection * 700 * FVector(1, 1, 0));
	Multicast_ApplyMeshImpulse(HitDirection * FVector(50, 50, 0));
}

void AMLCharacterBase::ApplyHitImpulse(FVector LocationFrom, FVector LocationTo, float ImpulseMultiplier)
{
	FVector HitDirection = LocationTo - LocationFrom;
	UKismetMathLibrary::Vector_Normalize(HitDirection);
	HitDirection *= 50 * ImpulseMultiplier;
	GetCharacterMovement()->AddImpulse(HitDirection * 700 * FVector(1, 1, 0));
	Multicast_ApplyMeshImpulse(HitDirection * FVector(50, 50, 0));
}

void AMLCharacterBase::ApplyHitImpulseAndMontageByDirection(FVector HitDirection, float ImpulseMultiplier)
{
	ApplyHitImpulseByDirection(HitDirection, ImpulseMultiplier);
	Multicast_PlayHitMontage(HitDirection);
}

void AMLCharacterBase::ApplyHitImpulseAndMontage(FVector LocationFrom, FVector LocationTo, float ImpulseMultiplier)
{
	ApplyHitImpulse(LocationFrom, LocationTo, ImpulseMultiplier);
	Multicast_PlayHitMontage(LocationTo - LocationFrom);
}

void AMLCharacterBase::Multicast_UseUpperBodyPoseSnapshotForDuration_Implementation(float Duration)
{
	UseUpperBodyPoseSnapshotForDuration(Duration);
}

UMLAttributeSet* AMLCharacterBase::GetAttributeSet() const
{
	return AttributeSet;
}

bool AMLCharacterBase::DoesHitStop() const
{
	return HitStops;
}

void AMLCharacterBase::Multicast_PlayHitMontage_Implementation(FVector HitDirection)
{
	PlayHitMontage(HitDirection);
}

UInventoryComponent* AMLCharacterBase::GetInventoryComponent_Implementation() const
{
	return EquipmentComponent;
}


// Copyright Epic Games, Inc. All Rights Reserved.

#include "Characters/MLCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Inventory/InventoryComponent.h"
#include "Inventory/Equipment/EquipmentComponent.h"
#include "AbilitySystem/MLAbilitySystemComponent.h"
#include "AbilitySystem/MLAttributeSet.h"
#include "AbilitySystem/MLGameplayAbility.h"
#include "MLPlayerController.h"
#include "TaggableSkeletalMeshComponent.h"
#include "Inventory/InventoryFunctions.h"
#include "MLUIWidget.h"
#include "Interactable.h"
#include "Inventory/Equipment/EquipmentDataAsset.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"



//////////////////////////////////////////////////////////////////////////
// AMedievalLooterCharacter
AMLCharacter::AMLCharacter()
{
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);


	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);


	LegsMesh = CreateDefaultSubobject<UTaggableSkeletalMeshComponent>(TEXT("LegsMesh"));
	LegsMesh->SetupAttachment(GetMesh());
	HeadMesh = CreateDefaultSubobject<UTaggableSkeletalMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(GetMesh());
	TorsoMesh = CreateDefaultSubobject<UTaggableSkeletalMeshComponent>(TEXT("TorsoMesh"));
	TorsoMesh->SetupAttachment(GetMesh());
	RightHandMesh = CreateDefaultSubobject<UTaggableSkeletalMeshComponent>(TEXT("RightHandMesh"));
	RightHandMesh->SetupAttachment(GetMesh(), FName(TEXT("RightHandSocket")));
	LeftHandMesh = CreateDefaultSubobject<UTaggableSkeletalMeshComponent>(TEXT("LeftHandMesh"));
	LeftHandMesh->SetupAttachment(GetMesh(), FName(TEXT("LeftHandSocket")));
	BackMesh = CreateDefaultSubobject<UTaggableSkeletalMeshComponent>(TEXT("BackMesh"));
	BackMesh->SetupAttachment(GetMesh(), FName(TEXT("BackSocket")));

	SkeletalMeshComponentReplicationRegister.Add(EMeshIdentifier::Legs, LegsMesh);
	SkeletalMeshComponentReplicationRegister.Add(EMeshIdentifier::Head, HeadMesh);
	SkeletalMeshComponentReplicationRegister.Add(EMeshIdentifier::Torso, TorsoMesh);
	SkeletalMeshComponentReplicationRegister.Add(EMeshIdentifier::RightHand, RightHandMesh);
	SkeletalMeshComponentReplicationRegister.Add(EMeshIdentifier::LeftHand, LeftHandMesh);
	SkeletalMeshComponentReplicationRegister.Add(EMeshIdentifier::BackRight, BackMesh);
}

void AMLCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	GetWorldTimerManager().SetTimer(InteractableTimerHandle, this, &AMLCharacter::UpdateInteractable, 0.2f, true, 0.f);


	//Ability System Attribute Change Callbacks
	if (AbilitySystemComponent)
	{
		HealthChangedUIDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data) {
			
			if (AMLPlayerController* PC = Cast<AMLPlayerController>(GetOwner()))
			{
				if (UMLUIWidget* UI = PC->GetUIWidget())
				{
					UI->SetHealth(Data.NewValue);
				}
			}

		});
		MaxHealthChangedUIDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data) {

			if (AMLPlayerController* PC = Cast<AMLPlayerController>(GetOwner()))
			{
				if (UMLUIWidget* UI = PC->GetUIWidget())
				{
					UI->SetMaxHealth(Data.NewValue);
				}
			}

		});
		StaminaChangedUIDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetStaminaAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data) {

			if (AMLPlayerController* PC = Cast<AMLPlayerController>(GetOwner()))
			{
				if (UMLUIWidget* UI = PC->GetUIWidget())
				{
					UI->SetStamina(Data.NewValue);
				}
			}

		});
		MaxStaminaChangedUIDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxStaminaAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& Data) {

			if (AMLPlayerController* PC = Cast<AMLPlayerController>(GetOwner()))
			{
				if (UMLUIWidget* UI = PC->GetUIWidget())
				{
					UI->SetMaxStamina(Data.NewValue);
				}
			}

		});
	}

	//Inventory Callbacks
	if (EquipmentComponent)
	{
		EquipmentComponent->InventoryChangedDelegate.AddDynamic(this, &AMLCharacter::OnInventoryChanged);
		EquipmentComponent->ItemEquippedDelegate.AddDynamic(this, &AMLCharacter::GrantEquipmentAbilities);
		EquipmentComponent->ItemUnequippedDelegate.AddDynamic(this, &AMLCharacter::RemoveEquipmentAbilities);
	}
}

void AMLCharacter::OnInventoryChanged() {
	if (AMLPlayerController* PC = Cast<AMLPlayerController>(GetOwner()))
	{
		if (UMLUIWidget* UI = PC->GetUIWidget())
		{
			UI->SetInventory(EquipmentComponent->GetInventory());
		}
	}
}


void AMLCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	GrantDefaultAbilities();
	if (AMLPlayerController* PC = Cast<AMLPlayerController>(GetController()))
	{
		PC->CreateUI();
	}
}

void AMLCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	if (AMLPlayerController* PC = Cast<AMLPlayerController>(GetController()))
	{
		PC->CreateUI();
	}
}

void AMLCharacter::GrantAbility(UPARAM(ref)const TSubclassOf<UMLGameplayAbility>& NewAbility)
{
	if (!HasAuthority() || !AbilitySystemComponent) { return; }

	EMLAbilityInputID inputID = NewAbility.GetDefaultObject()->InputID;

	if (CurrentAbilites.Contains(inputID))
	{
		AbilitySystemComponent->ClearAbility(CurrentAbilites[inputID].AbilitySpecHandle);
	}

	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(NewAbility, NewAbility.GetDefaultObject()->GetAbilityLevel(), static_cast<int32>(inputID), this);
	FGameplayAbilitySpecHandle NewSpecHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);
	FCurrentAbilityPair NewAbilityPair = FCurrentAbilityPair(NewAbility, NewSpecHandle);
	CurrentAbilites.Emplace(inputID, NewAbilityPair);
}

void AMLCharacter::RemoveAbility(UPARAM(ref)const TSubclassOf<UMLGameplayAbility>& OutAbility)
{
	if (!HasAuthority() || !AbilitySystemComponent) { return; }

	EMLAbilityInputID inputID = OutAbility.GetDefaultObject()->InputID;

	if (CurrentAbilites.Contains(inputID))
	{
		AbilitySystemComponent->ClearAbility(CurrentAbilites[inputID].AbilitySpecHandle);

		if (DefaultAbilities.Contains(inputID))
		{
			const TSubclassOf<UMLGameplayAbility>& DefaultAbility = DefaultAbilities[inputID];
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(DefaultAbility, DefaultAbility.GetDefaultObject()->GetAbilityLevel(), static_cast<int32>(inputID), this);
			FGameplayAbilitySpecHandle DefaultSpecHandle = AbilitySystemComponent->GiveAbility(AbilitySpec);
			FCurrentAbilityPair DefaultAbilityPair = FCurrentAbilityPair(DefaultAbility, DefaultSpecHandle);
			CurrentAbilites.Emplace(inputID, DefaultAbilityPair);
		}
	}
}

void AMLCharacter::GrantAbilities(UPARAM(ref)const TArray<TSubclassOf<UMLGameplayAbility>>& NewAbilities)
{
	if (!HasAuthority() || !AbilitySystemComponent) { return; }

	for (const TSubclassOf<UMLGameplayAbility>& NewAbility : NewAbilities)
	{
		GrantAbility(NewAbility);
	}
}

void AMLCharacter::RemoveAbilities(UPARAM(ref)const TArray<TSubclassOf<UMLGameplayAbility>>& OutAbilities)
{
	if (!HasAuthority() || !AbilitySystemComponent) { return; }

	for (const TSubclassOf<UMLGameplayAbility>& OutAbility : OutAbilities)
	{
		RemoveAbility(OutAbility);
	}
}

void AMLCharacter::GrantEquipmentAbilities(FInventoryItemInfo ItemInfo)
{
	UEquipmentDataAsset* EquipmentData = UInventoryFunctions::GetEquipmentData(ItemInfo);
	if (!EquipmentData) { return; }

	GrantAbilities(EquipmentData->GrantedAbilities);
}

void AMLCharacter::RemoveEquipmentAbilities(FInventoryItemInfo ItemInfo)
{
	UEquipmentDataAsset* EquipmentData = UInventoryFunctions::GetEquipmentData(ItemInfo);
	if (!EquipmentData) { return; }

	RemoveAbilities(EquipmentData->GrantedAbilities);
}

void AMLCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AMLCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMLCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMLCharacter::Look);

		EnhancedInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Triggered, this, &AMLCharacter::ToggleInventory, true);
		EnhancedInputComponent->BindAction(CloseInventoryAction, ETriggerEvent::Triggered, this, &AMLCharacter::ToggleInventory, false);

		EnhancedInputComponent->BindAction(OpenMapAction, ETriggerEvent::Triggered, this, &AMLCharacter::ToggleMap, true);
		EnhancedInputComponent->BindAction(CloseMapAction, ETriggerEvent::Triggered, this, &AMLCharacter::ToggleMap, false);

		EnhancedInputComponent->BindAction(OpenMenuAction, ETriggerEvent::Triggered, this, &AMLCharacter::ToggleMenu, true);
		EnhancedInputComponent->BindAction(CloseMenuAction, ETriggerEvent::Triggered, this, &AMLCharacter::ToggleMap, false);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AMLCharacter::ToggleCrouching, true);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AMLCharacter::ToggleCrouching, false);
	}

	SetupAbilitySystemInputBindings();

}

void AMLCharacter::SetupAbilitySystemInputBindings()
{
	UInputAction* ConfirmAction = nullptr;
	bool ConfirmPresent = false;
	UInputAction* CancelAction = nullptr;
	bool CancelPresent = false;
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent)) {

		if (InputActionAbilityBindings.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("ACTOR'S InputActionAbilityBindings MUST BE SET IN CHARACTER CHILD BP!!!"));
		}
		for (auto& Pair : InputActionAbilityBindings) {

			EMLAbilityInputID inputID = Pair.Key;
			UInputAction* inputAction = Pair.Value;

			if (inputID == EMLAbilityInputID::Confirm) {
				ConfirmPresent = true;
				ConfirmAction = inputAction;
				continue;
			}
			if (inputID == EMLAbilityInputID::Cancel) {
				CancelPresent = true;
				CancelAction = inputAction;
				continue;
			}

			EnhancedInputComponent->BindAction(inputAction, ETriggerEvent::Triggered, this, &AMLCharacter::SendInputToAbilitySystemComponent, true, inputID);
			EnhancedInputComponent->BindAction(inputAction, ETriggerEvent::Completed, this, &AMLCharacter::SendInputToAbilitySystemComponent, false, inputID);
		}

		if (!AbilitySystemComponent) { return; }

		if (ConfirmPresent) {
			AbilitySystemComponent->GenericConfirmInputID = static_cast<int32>(EMLAbilityInputID::Confirm);
			EnhancedInputComponent->BindAction(ConfirmAction, ETriggerEvent::Triggered, this, &AMLCharacter::SendInputToAbilitySystemComponent, true, EMLAbilityInputID::Confirm);
		}
		if (CancelPresent) {
			AbilitySystemComponent->GenericCancelInputID = static_cast<int32>(EMLAbilityInputID::Cancel);
			EnhancedInputComponent->BindAction(CancelAction, ETriggerEvent::Triggered, this, &AMLCharacter::SendInputToAbilitySystemComponent, true, EMLAbilityInputID::Cancel);
		}
			
	}
}

void AMLCharacter::SendInputToAbilitySystemComponent(bool isPressed, EMLAbilityInputID inputID)
{
	if (!AbilitySystemComponent) { return; }

	if (inputID == EMLAbilityInputID::Confirm)
	{
		AbilitySystemComponent->LocalInputConfirm();
	}
	else if (inputID == EMLAbilityInputID::Cancel)
	{
		AbilitySystemComponent->LocalInputCancel();
	}
	else if (isPressed)
	{
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(inputID));
	}
	else
	{
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(inputID));
	}
}


void AMLCharacter::GrantDefaultAbilities()
{
	if (!HasAuthority() || !AbilitySystemComponent) { return; }

	for (auto& Ability : DefaultAbilities)
	{
		GrantAbility(Ability.Value);
	}
}

void AMLCharacter::UpdateInteractableUI()
{
	if (AMLPlayerController* PC = Cast<AMLPlayerController>(GetOwner()))
	{
		if (UMLUIWidget* UI = PC->GetUIWidget())
		{
			UI->SetInteractableText(InteractableText);
		}
	}
}

void AMLCharacter::UpdateInteractable()
{
	FVector StartLineTrace = GetActorLocation() + FVector(0, 0, 70);
	FVector EndLineTrace = StartLineTrace + FollowCamera->GetForwardVector() * 400;

	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);

	UWorld* World = GetWorld();

	if (!World) { return; }

	World->LineTraceSingleByChannel(HitResult, StartLineTrace, EndLineTrace, ECollisionChannel::ECC_Visibility, CollisionQueryParams);
	FVector InteractSphereSpawnLocation = HitResult.Location;

	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery2);
	TArray<AActor*> ActorsToIgnore = TArray<AActor*>();
	ActorsToIgnore.Emplace(this);

	UKismetSystemLibrary::SphereOverlapActors(World, InteractSphereSpawnLocation, 100, ObjectTypes, nullptr, ActorsToIgnore, OutActors);

	for (AActor* Actor : OutActors)
	{
		if (IInteractable* ActorAsInteractable = Cast<IInteractable>(Actor))
		{
			InteractableText = ActorAsInteractable->GetInteractionText();
			UpdateInteractableUI();
			if (AbilitySystemComponent)
			{
				AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.CanInteract")));
			}
			return;
		}
	}
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Ability.CanInteract")));
	}
	InteractableText = FText::FromString("");
	UpdateInteractableUI();
}

void AMLCharacter::ToggleCrouching(bool ShouldCrouch)
{
	if (ShouldCrouch)
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

void AMLCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		FVector2D NormalizedMovement = MovementVector.GetSafeNormal();
		double DotProduct = FVector2D::DotProduct(FVector2D(0,1), NormalizedMovement);
		DotProduct = DotProduct + 5;
		double MovementMultiplier = DotProduct / 6;

		AddMovementInput(ForwardDirection, MovementVector.Y * MovementMultiplier);
		AddMovementInput(RightDirection, MovementVector.X * MovementMultiplier);
	}
}

void AMLCharacter::Look(const FInputActionValue& Value)
{

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMLCharacter::ToggleInventory(bool ShowInventory)
{
	if (AMLPlayerController* PC = Cast<AMLPlayerController>(GetOwner()))
	{
		if (UMLUIWidget* UI = PC->GetUIWidget())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				if (ShowInventory)
				{
					Subsystem->RemoveMappingContext(DefaultMappingContext);
					Subsystem->AddMappingContext(InventoryMappingContext, 0);
				}
				else
				{
					Subsystem->RemoveMappingContext(InventoryMappingContext);
					Subsystem->AddMappingContext(DefaultMappingContext, 0);
				}
				
			}
			UI->ToggleInventory(ShowInventory);
		}
	}
}

void AMLCharacter::ToggleMap(bool ShowMap)
{
	if (AMLPlayerController* PC = Cast<AMLPlayerController>(GetOwner()))
	{
		if (UMLUIWidget* UI = PC->GetUIWidget())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				if (ShowMap)
				{
					Subsystem->RemoveMappingContext(DefaultMappingContext);
					Subsystem->AddMappingContext(MapMappingContext, 0);
				}
				else
				{
					Subsystem->RemoveMappingContext(MapMappingContext);
					Subsystem->AddMappingContext(DefaultMappingContext, 0);
				}

			}
			UI->ToggleMap(ShowMap);
		}
	}
}

void AMLCharacter::ToggleMenu(bool ShowMenu)
{
	if (AMLPlayerController* PC = Cast<AMLPlayerController>(GetOwner()))
	{
		if (UMLUIWidget* UI = PC->GetUIWidget())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
			{
				if (ShowMenu)
				{
					Subsystem->RemoveMappingContext(DefaultMappingContext);
					Subsystem->AddMappingContext(MenuMappingContext, 0);
				}
				else
				{
					Subsystem->RemoveMappingContext(MenuMappingContext);
					Subsystem->AddMappingContext(DefaultMappingContext, 0);
				}

			}
			UI->ToggleMenu(ShowMenu);
		}
	}
}







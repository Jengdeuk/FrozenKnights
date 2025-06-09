// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FKCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FKCharacterControlData.h"
#include "Engine/AssetManager.h"
#include "Player/FKPlayerController.h"
#include "Player/FKPlayerState.h"
#include "ProjectFK.h"
#include "EngineUtils.h"
#include "Components/WidgetComponent.h"
#include "Physics/FKCollision.h"
#include "Components/CapsuleComponent.h"

AFKCharacterPlayer::AFKCharacterPlayer()
{
	bPlayerCharacter = true;

	// Capsule
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_FKCAPSULE);

	// Camera
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Input
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Script/EnhancedInput.InputAction'/Game/FrozenKnights/Input/Action/IA_Jump.IA_Jump'"));
	if (nullptr != InputActionJumpRef.Object)
	{
		JumpAction = InputActionJumpRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderMoveRef(TEXT("/Script/EnhancedInput.InputAction'/Game/FrozenKnights/Input/Action/IA_ShoulderMove.IA_ShoulderMove'"));
	if (nullptr != InputActionShoulderMoveRef.Object)
	{
		ShoulderMoveAction = InputActionShoulderMoveRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionShoulderLookRef(TEXT("/Script/EnhancedInput.InputAction'/Game/FrozenKnights/Input/Action/IA_ShoulderLook.IA_ShoulderLook'"));
	if (nullptr != InputActionShoulderLookRef.Object)
	{
		ShoulderLookAction = InputActionShoulderLookRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionAttackRef(TEXT("/Script/EnhancedInput.InputAction'/Game/FrozenKnights/Input/Action/IA_Attack.IA_Attack'"));
	if (nullptr != InputActionAttackRef.Object)
	{
		AttackAction = InputActionAttackRef.Object;
	}

	CurrentCharacterControlType = ECharacterControlType::Shoulder;

	// Helm Component
	Helm = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Helm"));
	Helm->SetupAttachment(GetMesh(), TEXT("head"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> HelmMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/ParagonGreystone/Characters/Heroes/Greystone/Skins/WhiteTiger/Meshes/SM_Greystone_TigerHelm.SM_Greystone_TigerHelm'"));
	if (HelmMeshRef.Object)
	{
		HelmMesh = HelmMeshRef.Object;
	}

	OnMeshLoadCompleted.AddUObject(this, &AFKCharacterPlayer::OnInitMeshCompleted);
	OnDead.AddUObject(this, &AFKCharacterPlayer::SetDead);
}

void AFKCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetCharacterControl(CurrentCharacterControlType);
}

void AFKCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	UpdateMeshFromPlayerState();
}

void AFKCharacterPlayer::SetDead()
{
	Super::SetDead();

	if (IsLocallyControlled())
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController)
		{
			DisableInput(PlayerController);
		}
	}
}

void AFKCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(ShoulderMoveAction, ETriggerEvent::Triggered, this, &AFKCharacterPlayer::ShoulderMove);
	EnhancedInputComponent->BindAction(ShoulderLookAction, ETriggerEvent::Triggered, this, &AFKCharacterPlayer::ShoulderLook);
}

bool AFKCharacterPlayer::ServerRPCPlayAttackMontage_Validate()
{
	return true;
}

void AFKCharacterPlayer::ServerRPCPlayAttackMontage_Implementation()
{
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if (!PlayerController->IsLocalController())
			{
				AFKCharacterPlayer* OtherPlayer = Cast<AFKCharacterPlayer>(PlayerController->GetPawn());
				if (OtherPlayer)
				{
					OtherPlayer->ClientRPCPlayAttackMontage(this);
				}
			}
		}
	}
}

void AFKCharacterPlayer::ClientRPCPlayAttackMontage_Implementation(AFKCharacterPlayer* CharacterToPlay)
{
	if (CharacterToPlay)
	{
		CharacterToPlay->PlayAttackMontage();
	}
}

bool AFKCharacterPlayer::ServerRPCMontageJumpToSection_Validate(FName SectionName)
{
	return true;
}

void AFKCharacterPlayer::ServerRPCMontageJumpToSection_Implementation(FName SectionName)
{
	for (APlayerController* PlayerController : TActorRange<APlayerController>(GetWorld()))
	{
		if (PlayerController && GetController() != PlayerController)
		{
			if (!PlayerController->IsLocalController())
			{
				AFKCharacterPlayer* OtherPlayer = Cast<AFKCharacterPlayer>(PlayerController->GetPawn());
				if (OtherPlayer)
				{
					OtherPlayer->ClientRPCMontageJumpToSection(this, SectionName);
				}
			}
		}
	}
}

void AFKCharacterPlayer::ClientRPCMontageJumpToSection_Implementation(AFKCharacterPlayer* CharacterToPlay, FName SectionName)
{
	if (CharacterToPlay)
	{
		CharacterToPlay->JumpMontageToSection(SectionName);
	}
}

void AFKCharacterPlayer::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(AttackMontage);
}

void AFKCharacterPlayer::JumpMontageToSection(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_JumpToSection(SectionName);
}

void AFKCharacterPlayer::SetCharacterControl(ECharacterControlType NewCharacterControlType)
{
	if (!IsLocallyControlled())
	{
		return;
	}

	UFKCharacterControlData* NewCharacterControl = CharacterControlManager[NewCharacterControlType];
	check(NewCharacterControl);

	SetCharacterControlData(NewCharacterControl);

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->ClearAllMappings();
		UInputMappingContext* NewMappingContext = NewCharacterControl->InputMappingContext;
		if (NewMappingContext)
		{
			Subsystem->AddMappingContext(NewMappingContext, 0);
		}
	}

	CurrentCharacterControlType = NewCharacterControlType;
}

void AFKCharacterPlayer::SetCharacterControlData(const UFKCharacterControlData* CharacterControlData)
{
	Super::SetCharacterControlData(CharacterControlData);

	CameraBoom->TargetArmLength = CharacterControlData->TargetArmLength;
	CameraBoom->SetRelativeRotation(CharacterControlData->RelativeRotation);
	CameraBoom->bUsePawnControlRotation = CharacterControlData->bUsePawnControlRotation;
	CameraBoom->bInheritPitch = CharacterControlData->bInheritPitch;
	CameraBoom->bInheritYaw = CharacterControlData->bInheritYaw;
	CameraBoom->bInheritRoll = CharacterControlData->bInheritRoll;
	CameraBoom->bDoCollisionTest = CharacterControlData->bDoCollisionTest;
}

void AFKCharacterPlayer::ShoulderMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
}

void AFKCharacterPlayer::ShoulderLook(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void AFKCharacterPlayer::UpdateMeshFromPlayerState()
{
	AFKPlayerState* PS = GetPlayerState<AFKPlayerState>();
	EPlayerClass PlayerClass = PS->GetPlayerClass();
	if (PlayerClass == EPlayerClass::None)
	{
		return;
	}
	
	MeshHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(PlayerMeshes[uint8(PlayerClass)], FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::MeshLoadCompleted));
	AnimHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(PlayerAnimInstances[uint8(PlayerClass)], FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::AnimLoadCompleted));
	AttackHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(PlayerAttackMontages[uint8(PlayerClass)], FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::AttackMontageLoadCompleted));
	DeadHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(PlayerDeadMontages[uint8(PlayerClass)], FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::DeadMontageLoadCompleted));
	ComboActionHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(PlayerComboActionData[uint8(PlayerClass)], FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::ComboActionDataLoadCompleted));
}

void AFKCharacterPlayer::OnInitMeshCompleted()
{
	AFKPlayerState* PS = GetPlayerState<AFKPlayerState>();
	EPlayerClass PlayerClass = PS->GetPlayerClass();
	if (PlayerClass == EPlayerClass::Knight)
	{
		EquipHelm();
	}

	HpBar->SetHiddenInGame(false);

	if (IsLocallyControlled())
	{
		AFKPlayerController* PlayerController = CastChecked<AFKPlayerController>(GetController());
		PlayerController->StartGame();
	}
}

void AFKCharacterPlayer::EquipHelm()
{
	Helm->SetStaticMesh(HelmMesh);
	Helm->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
}

void AFKCharacterPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	UpdateMeshFromPlayerState();
}

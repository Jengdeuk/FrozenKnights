// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FKCharacterPlayer.h"
#include "ProjectFK.h"
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
#include "EngineUtils.h"
#include "Components/WidgetComponent.h"
#include "Physics/FKCollision.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"

AFKCharacterPlayer::AFKCharacterPlayer()
{
	bPlayerCharacter = true;

	DeactivateDuration = 5.0f;

	// Respawn
	bIsWaitingForRespawn = false;
	RespawnStartTime = 0.0f;
	RespawnDuration = 10.0f;

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

	// Collider Component
	CharacterOverlapDetector = CreateDefaultSubobject<USphereComponent>(TEXT("CharacterOverlapDetector"));
	CharacterOverlapDetector->SetupAttachment(RootComponent);
	CharacterOverlapDetector->SetSphereRadius(1250.0f); // HPBar 표시 범위
	CharacterOverlapDetector->SetCollisionProfileName(CPROFILE_FKCHARACTEROVERLAPDETECTOR);
	CharacterOverlapDetector->SetGenerateOverlapEvents(true);
	CharacterOverlapDetector->SetShouldUpdatePhysicsVolume(false);

	CharacterOverlapDetector->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnCharacterOverlapBegin);
	CharacterOverlapDetector->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnCharacterOverlapEnd);
}

void AFKCharacterPlayer::Activate()
{
	Super::Activate();

	bIsWaitingForRespawn = false;
	if (HasAuthority())
	{
		RespawnTimerHandle.Invalidate();
	}

	if (IsLocallyControlled())
	{
		AFKPlayerController* PlayerController = CastChecked<AFKPlayerController>(GetController());
		PlayerController->StartGame();
	}
}

void AFKCharacterPlayer::Deactivate()
{
	Super::Deactivate();
	
	if (HasAuthority())
	{
		DeactiveTimerHandle.Invalidate();
	}
}

void AFKCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();

	SetCharacterControl(CurrentCharacterControlType);
}

void AFKCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	BindCharacterResources();
}

void AFKCharacterPlayer::SetDead()
{
	Super::SetDead();

	bIsWaitingForRespawn = true;
	RespawnStartTime = GetWorld()->GetTimeSeconds();
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(DeactiveTimerHandle, this, &ThisClass::Deactivate, DeactivateDuration, false);
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ThisClass::DeferredActivate, RespawnDuration, false);
	}

	AFKPlayerController* PlayerController = Cast<AFKPlayerController>(GetController());
	if (!PlayerController)
		return;

	if (PlayerController->IsLocalController())
	{
		PlayerController->PauseGame();
		PlayerController->PreparingToReturn();
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

	SummonCastMageEffect();
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

	SummonCastMageEffect();
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

void AFKCharacterPlayer::SummonCastMageEffect()
{
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

void AFKCharacterPlayer::BindCharacterResources()
{
	AFKPlayerState* PS = GetPlayerState<AFKPlayerState>();
	EPlayerClass PlayerClass = PS->GetPlayerClass();
	if (PlayerClass == EPlayerClass::None)
	{
		return;
	}

	bResourceBinds.FindOrAdd(EResourceType::Mesh) = false;
	ResourceHandles.FindOrAdd(EResourceType::Mesh) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		Meshes[uint8(PlayerClass)],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::MeshLoadCompleted)
	);

	bResourceBinds.FindOrAdd(EResourceType::AnimInstance) = false;
	ResourceHandles.FindOrAdd(EResourceType::AnimInstance) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		AnimInstances[uint8(PlayerClass)],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::AnimLoadCompleted)
	);

	bResourceBinds.FindOrAdd(EResourceType::StartMontage) = false;
	ResourceHandles.FindOrAdd(EResourceType::StartMontage) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		StartMontages[uint8(PlayerClass)],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::StartMontageLoadCompleted)
	);

	bResourceBinds.FindOrAdd(EResourceType::AttackMontage) = false;
	ResourceHandles.FindOrAdd(EResourceType::AttackMontage) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		AttackMontages[uint8(PlayerClass)],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::AttackMontageLoadCompleted)
	);

	bResourceBinds.FindOrAdd(EResourceType::DeadMontage) = false;
	ResourceHandles.FindOrAdd(EResourceType::DeadMontage) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		DeadMontages[uint8(PlayerClass)],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::DeadMontageLoadCompleted)
	);

	bResourceBinds.FindOrAdd(EResourceType::ComboActionData) = false;
	ResourceHandles.FindOrAdd(EResourceType::ComboActionData) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		ComboActionDatas[uint8(PlayerClass)],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::ComboActionDataLoadCompleted)
	);
}

void AFKCharacterPlayer::OnBindResourcesCompleted()
{
	Super::OnBindResourcesCompleted();

	if (IsLocallyControlled())
	{
		ServerRPCDeferredActivate();
	}

	if (AFKPlayerState* PS = GetPlayerState<AFKPlayerState>())
	{
		EPlayerClass PlayerClass = PS->GetPlayerClass();
		if (PlayerClass == EPlayerClass::Knight)
		{
			EquipHelm();
		}
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

	BindCharacterResources();
}

void AFKCharacterPlayer::OnCharacterOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsLocallyControlled() == false)
	{
		return;
	}

	AFKCharacterBase* OtherChar = Cast<AFKCharacterBase>(OtherActor);
	if (OtherChar && OtherChar != this && OtherChar->IsActive())
	{
		OtherChar->SetHpBarHiddenInGame(false);
	}
}

void AFKCharacterPlayer::OnCharacterOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsLocallyControlled() == false)
	{
		return;
	}

	AFKCharacterBase* OtherChar = Cast<AFKCharacterBase>(OtherActor);
	if (OtherChar && OtherChar != this)
	{
		OtherChar->SetHpBarHiddenInGame(true);
	}
}
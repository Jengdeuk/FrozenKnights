// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FKCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Physics/FKCollision.h"
#include "FKCharacterControlData.h"
#include "Player/FKPlayerController.h"
#include "Engine/AssetManager.h"
#include "Character/FKComboActionData.h"
#include "Net/UnrealNetwork.h"

AFKCharacterBase::AFKCharacterBase()
{
	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_FKCAPSULE);

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 700.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	GetMesh()->SetHiddenInGame(true);

	bReplicates = true;
	bDead = false;
}

void AFKCharacterBase::SetCharacterControlData(const UFKCharacterControlData* CharacterControlData)
{
	// Pawn
	bUseControllerRotationYaw = CharacterControlData->bUseControllerRotationYaw;

	// CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = CharacterControlData->bOrientRotationToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = CharacterControlData->bUseControllerDesiredRotation;
	GetCharacterMovement()->RotationRate = CharacterControlData->RotationRate;
}

void AFKCharacterBase::MeshLoadCompleted()
{
	if (MeshHandle.IsValid())
	{
		USkeletalMesh* NewMesh = Cast<USkeletalMesh>(MeshHandle->GetLoadedAsset());
		if (NewMesh)
		{
			GetMesh()->SetSkeletalMesh(NewMesh);
		}
	}

	MeshHandle->ReleaseHandle();
}

void AFKCharacterBase::AnimLoadCompleted()
{
	if (AnimHandle.IsValid())
	{
		UClass* NewAnim = Cast<UClass>(AnimHandle->GetLoadedAsset());
		if (NewAnim)
		{
			GetMesh()->SetAnimInstanceClass(NewAnim);
			GetMesh()->SetHiddenInGame(false);
			OnMeshLoadCompleted.Broadcast();
		}
	}

	AnimHandle->ReleaseHandle();
}

void AFKCharacterBase::AttackMontageLoadCompleted()
{
	if (AttackHandle.IsValid())
	{
		AttackMontage = Cast<UAnimMontage>(AttackHandle->GetLoadedAsset());
	}

	AttackHandle->ReleaseHandle();
}

void AFKCharacterBase::DeadMontageLoadCompleted()
{
	if (DeadHandle.IsValid())
	{
		DeadMontage = Cast<UAnimMontage>(DeadHandle->GetLoadedAsset());
	}

	DeadHandle->ReleaseHandle();
}

void AFKCharacterBase::ComboActionDataLoadCompleted()
{
	if (ComboActionHandle.IsValid())
	{
		ComboActionData = Cast<UFKComboActionData>(ComboActionHandle->GetLoadedAsset());
	}

	ComboActionHandle->ReleaseHandle();
}

void AFKCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFKCharacterBase, bDead);
}

void AFKCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
}

void AFKCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->StopAllMontages(0.0f);
	AnimInstance->Montage_Play(DeadMontage, 1.0f);
}

void AFKCharacterBase::OnRep_Dead()
{
	OnDead.Broadcast();
}
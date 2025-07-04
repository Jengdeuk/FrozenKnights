// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FKCharacterBase.h"
#include "ProjectFK.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Physics/FKCollision.h"
#include "FKCharacterControlData.h"
#include "Player/FKPlayerController.h"
#include "Engine/AssetManager.h"
#include "Character/FKComboActionData.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "EngineUtils.h"

AFKCharacterBase::AFKCharacterBase()
{
	bPlayerCharacter = false;
	bDead = true;
	bActive = false;
	bReplicates = true;
	bAlwaysRelevant = true;

	// Pawn
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.05f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 6000.f;
	GetCharacterMovement()->BrakingFrictionFactor = 0.3f;
	GetCharacterMovement()->GravityScale = 2.0f;

	// Mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -100.0f), FRotator(0.0f, -90.0f, 0.0f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	GetMesh()->SetHiddenInGame(true);

	OnResourcesBindCompleted.AddUObject(this, &ThisClass::OnBindResourcesCompleted);
	OnActiveChanged.AddUObject(this, &ThisClass::ChangeActive);
	OnDead.AddUObject(this, &ThisClass::SetDead);
}

bool AFKCharacterBase::ServerRPCDeferredActivate_Validate()
{
	return true;
}

void AFKCharacterBase::ServerRPCDeferredActivate_Implementation()
{
	DeferredActivate();
}

void AFKCharacterBase::DeferredActivate()
{
	if (HasAuthority())
	{
		MulticastRPCPlayStartMontage();
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ThisClass::Activate, 5.0f, false);
	}
}

void AFKCharacterBase::Activate()
{
	if (HasAuthority())
	{
		bActive = true;
		RespawnTimerHandle.Invalidate();
	}

	SetActorHiddenInGame(false);
	GetMesh()->SetHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	ResetCharacterAnimation();
}

void AFKCharacterBase::Deactivate()
{
	if (HasAuthority())
	{
		bActive = false;
	}

	SetActorHiddenInGame(true);
	HpBar->SetHiddenInGame(true);
	GetMesh()->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
}

void AFKCharacterBase::SetHpBarHiddenInGame(bool bHiddenValue)
{
	HpBar->SetHiddenInGame(bHiddenValue);
}

void AFKCharacterBase::MulticastRPCPlayStartMontage_Implementation()
{
	PlayStartAnimation();
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

void AFKCharacterBase::OnBindResourcesCompleted()
{
}

bool AFKCharacterBase::CheckResourcesBindCompleted()
{
	for (const auto bResourceBind : bResourceBinds)
	{
		if (bResourceBind.Value == false)
		{
			return false;
		}
	}

	return true;
}

void AFKCharacterBase::MeshLoadCompleted()
{
	EResourceType ResourceType = EResourceType::Mesh;
	if (ResourceHandles[ResourceType].IsValid())
	{
		USkeletalMesh* NewMesh = Cast<USkeletalMesh>(ResourceHandles[ResourceType]->GetLoadedAsset());
		if (NewMesh)
		{
			bResourceBinds[ResourceType] = true;
			GetMesh()->SetSkeletalMesh(NewMesh);
			if (CheckResourcesBindCompleted())
			{
				OnResourcesBindCompleted.Broadcast();
			}
		}
	}

	ResourceHandles[ResourceType]->ReleaseHandle();
}

void AFKCharacterBase::AnimLoadCompleted()
{
	EResourceType ResourceType = EResourceType::AnimInstance;
	if (ResourceHandles[ResourceType].IsValid())
	{
		UClass* NewAnim = Cast<UClass>(ResourceHandles[ResourceType]->GetLoadedAsset());
		if (NewAnim)
		{
			bResourceBinds[ResourceType] = true;
			GetMesh()->SetAnimInstanceClass(NewAnim);
			if (CheckResourcesBindCompleted())
			{
				OnResourcesBindCompleted.Broadcast();
			}
		}
	}

	ResourceHandles[ResourceType]->ReleaseHandle();
}

void AFKCharacterBase::StartMontageLoadCompleted()
{
	EResourceType ResourceType = EResourceType::StartMontage;
	if (ResourceHandles[ResourceType].IsValid())
	{
		bResourceBinds[ResourceType] = true;
		StartMontage = Cast<UAnimMontage>(ResourceHandles[ResourceType]->GetLoadedAsset());
		if (CheckResourcesBindCompleted())
		{
			OnResourcesBindCompleted.Broadcast();
		}
	}

	ResourceHandles[ResourceType]->ReleaseHandle();
}

void AFKCharacterBase::AttackMontageLoadCompleted()
{
	EResourceType ResourceType = EResourceType::AttackMontage;
	if (ResourceHandles[ResourceType].IsValid())
	{
		bResourceBinds[ResourceType] = true;
		AttackMontage = Cast<UAnimMontage>(ResourceHandles[ResourceType]->GetLoadedAsset());
		if (CheckResourcesBindCompleted())
		{
			OnResourcesBindCompleted.Broadcast();
		}
	}

	ResourceHandles[ResourceType]->ReleaseHandle();
}

void AFKCharacterBase::DeadMontageLoadCompleted()
{
	EResourceType ResourceType = EResourceType::DeadMontage;
	if (ResourceHandles[ResourceType].IsValid())
	{
		bResourceBinds[ResourceType] = true;
		DeadMontage = Cast<UAnimMontage>(ResourceHandles[ResourceType]->GetLoadedAsset());
		if (CheckResourcesBindCompleted())
		{
			OnResourcesBindCompleted.Broadcast();
		}
	}

	ResourceHandles[ResourceType]->ReleaseHandle();
}

void AFKCharacterBase::ComboActionDataLoadCompleted()
{
	EResourceType ResourceType = EResourceType::ComboActionData;
	if (ResourceHandles[ResourceType].IsValid())
	{
		bResourceBinds[ResourceType] = true;
		ComboActionData = Cast<UFKComboActionData>(ResourceHandles[ResourceType]->GetLoadedAsset());
		if (CheckResourcesBindCompleted())
		{
			OnResourcesBindCompleted.Broadcast();
		}
	}

	ResourceHandles[ResourceType]->ReleaseHandle();
}

void AFKCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFKCharacterBase, bDead);
	DOREPLIFETIME(AFKCharacterBase, bActive);
}

void AFKCharacterBase::SetDead()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	PlayDeadAnimation();
	SetActorEnableCollision(false);
	HpBar->SetHiddenInGame(true);
}

void AFKCharacterBase::ResetCharacterAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(0.0f);
	}
}

void AFKCharacterBase::PlayStartAnimation()
{
	SetActorHiddenInGame(false);
	GetMesh()->SetHiddenInGame(false);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(0.0f);
		AnimInstance->Montage_Play(StartMontage, 1.0f);
	}
}

void AFKCharacterBase::PlayDeadAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(0.0f);
		AnimInstance->Montage_Play(DeadMontage, 1.0f);
	}
}

void AFKCharacterBase::OnRep_Dead()
{
	if (bDead)
	{
		OnDead.Broadcast();
	}
}

float AFKCharacterBase::GetRespawnRemainingTime() const
{
	if (bIsWaitingForRespawn == false)
	{
		return 0.0f;
	}

	float Elapsed = GetWorld()->GetTimeSeconds() - RespawnStartTime;
	return FMath::Clamp(RespawnDuration - Elapsed, 0.0f, RespawnDuration);
}

void AFKCharacterBase::ChangeActive()
{
	if (bActive)
	{
		Activate();
	}
	else
	{
		Deactivate();
	}
}

void AFKCharacterBase::OnRep_ActiveChanged()
{
	OnActiveChanged.Broadcast();
}

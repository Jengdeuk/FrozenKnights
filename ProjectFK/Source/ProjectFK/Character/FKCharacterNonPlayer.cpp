// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FKCharacterNonPlayer.h"
#include "Engine/AssetManager.h"
#include "Net/UnrealNetwork.h"
#include "Physics/FKCollision.h"
#include "Components/CapsuleComponent.h"
#include "AI/FKAIController.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Game/FKMonsterPoolManager.h"

AFKCharacterNonPlayer::AFKCharacterNonPlayer()
{
	NPCClass = ENPCClass::Mob;

	// Capsule
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_FKMOBCAPSULE);

	// Movement
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
}

void AFKCharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	bResourceBinds.FindOrAdd(EResourceType::Mesh) = false;
	ResourceHandles.FindOrAdd(EResourceType::Mesh) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		Meshes[uint8(NPCClass)],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::MeshLoadCompleted)
	);

	bResourceBinds.FindOrAdd(EResourceType::AnimInstance) = false;
	ResourceHandles.FindOrAdd(EResourceType::AnimInstance) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		AnimInstances[uint8(NPCClass)],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::AnimLoadCompleted)
	);

	bResourceBinds.FindOrAdd(EResourceType::AttackMontage) = false;
	ResourceHandles.FindOrAdd(EResourceType::AttackMontage) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		AttackMontages[uint8(NPCClass)],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::AttackMontageLoadCompleted)
	);

	bResourceBinds.FindOrAdd(EResourceType::DeadMontage) = false;
	ResourceHandles.FindOrAdd(EResourceType::DeadMontage) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		DeadMontages[uint8(NPCClass)],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::DeadMontageLoadCompleted)
	);

	bResourceBinds.FindOrAdd(EResourceType::ComboActionData) = false;
	ResourceHandles.FindOrAdd(EResourceType::ComboActionData) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		ComboActionDatas[uint8(NPCClass)],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::ComboActionDataLoadCompleted)
	);
}

void AFKCharacterNonPlayer::SetDead()
{
	Super::SetDead();

	if (HasAuthority())
	{
		AFKAIController* FKAIController = Cast<AFKAIController>(GetController());
		if (FKAIController)
		{
			FKAIController->StopAI();
		}

		GetWorld()->GetTimerManager().SetTimer(DeactiveTimerHandle, this, &ThisClass::Deactivate, 5.0f, false);

		if (PoolManager.IsValid())
		{
			PoolManager->DeferredSpawn(MonsterId);
		}
	}
}

void AFKCharacterNonPlayer::ActivatePoolableMonster(uint32 InMonsterId, AFKMonsterPoolManager* InPoolManager)
{
	if (HasAuthority() && InPoolManager)
	{
		MonsterId = InMonsterId;
		PoolManager = InPoolManager;
		if (CheckResourcesBindCompleted())
		{
			Activate();
		}
	}
}

void AFKCharacterNonPlayer::SetHomePos(FVector InPosition)
{
	AFKAIController* FKAIController = Cast<AFKAIController>(GetController());
	if (FKAIController)
	{
		FKAIController->SetHomePos(InPosition);
	}
}

void AFKCharacterNonPlayer::Activate()
{
	Super::Activate();

	if (HasAuthority())
	{
		AFKAIController* FKAIController = Cast<AFKAIController>(GetController());
		if (FKAIController)
		{
			FKAIController->RunAI();
		}
	}
}

void AFKCharacterNonPlayer::Deactivate()
{
	Super::Deactivate();

	if (HasAuthority())
	{
		DeactiveTimerHandle.Invalidate();

		AFKAIController* FKAIController = Cast<AFKAIController>(GetController());
		if (FKAIController)
		{
			FKAIController->StopAI();
		}
	}
}

void AFKCharacterNonPlayer::OnBindResourcesCompleted()
{
	Super::OnBindResourcesCompleted();
}

float AFKCharacterNonPlayer::GetAIPatrolRadius()
{
	return 800.0f;
}

float AFKCharacterNonPlayer::GetAIDetectRange()
{
	return 400.0f;
}

float AFKCharacterNonPlayer::GetAIAttackRange()
{
	return 0.0f;
}

float AFKCharacterNonPlayer::GetAITurnSpeed()
{
	return 0.0f;
}

void AFKCharacterNonPlayer::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
}

void AFKCharacterNonPlayer::AttackByAI()
{
}
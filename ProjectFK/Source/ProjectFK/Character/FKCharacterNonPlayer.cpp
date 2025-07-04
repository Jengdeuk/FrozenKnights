// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FKCharacterNonPlayer.h"
#include "ProjectFK.h"
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
	NPCType = ENPCType::Warchief;
	bPreparingActivate = false;
	bAlreadayResourcesBound = false;

	DeactivateDuration = 5.0f;

	// Capsule
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_FKMOBCAPSULE);

	ResourceSets.Add(ENPCType::Warchief, { 0, 0, 0, 0, 0, 0 });
	ResourceSets.Add(ENPCType::Qilin, { 1, 0, 0, 0, 0, 0 });
	ResourceSets.Add(ENPCType::BeetleRed, { 2, 0, 0, 0, 0, 0 });
}

void AFKCharacterNonPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFKCharacterNonPlayer, NPCType);
	DOREPLIFETIME(AFKCharacterNonPlayer, bAlreadayResourcesBound);
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

		GetWorld()->GetTimerManager().SetTimer(DeactiveTimerHandle, this, &ThisClass::Deactivate, DeactivateDuration, false);

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
		bPreparingActivate = true;
		MonsterId = InMonsterId;
		PoolManager = InPoolManager;
		BindCharacterResources();
	}
}

void AFKCharacterNonPlayer::SetType(ENPCType NewType)
{
	NPCType = NewType;
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
		bPreparingActivate = false;

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
		bAlreadayResourcesBound = false;
		DeactiveTimerHandle.Invalidate();

		AFKAIController* FKAIController = Cast<AFKAIController>(GetController());
		if (FKAIController)
		{
			FKAIController->StopAI();
		}
	}
}

void AFKCharacterNonPlayer::BindCharacterResources()
{
	if (HasAuthority())
	{
		bAlreadayResourcesBound = true;
	}

	bResourceBinds.FindOrAdd(EResourceType::Mesh) = false;
	ResourceHandles.FindOrAdd(EResourceType::Mesh) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		Meshes[ResourceSets[NPCType].MeshIndex],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::MeshLoadCompleted)
	);

	bResourceBinds.FindOrAdd(EResourceType::AnimInstance) = false;
	ResourceHandles.FindOrAdd(EResourceType::AnimInstance) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		AnimInstances[ResourceSets[NPCType].AnimInstanceIndex],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::AnimLoadCompleted)
	);

	bResourceBinds.FindOrAdd(EResourceType::StartMontage) = false;
	ResourceHandles.FindOrAdd(EResourceType::StartMontage) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		StartMontages[ResourceSets[NPCType].StartMontageIndex],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::StartMontageLoadCompleted)
	);

	bResourceBinds.FindOrAdd(EResourceType::AttackMontage) = false;
	ResourceHandles.FindOrAdd(EResourceType::AttackMontage) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		AttackMontages[ResourceSets[NPCType].AttackMontageIndex],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::AttackMontageLoadCompleted)
	);

	bResourceBinds.FindOrAdd(EResourceType::DeadMontage) = false;
	ResourceHandles.FindOrAdd(EResourceType::DeadMontage) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		DeadMontages[ResourceSets[NPCType].DeadMontageIndex],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::DeadMontageLoadCompleted)
	);

	bResourceBinds.FindOrAdd(EResourceType::ComboActionData) = false;
	ResourceHandles.FindOrAdd(EResourceType::ComboActionData) = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
		ComboActionDatas[ResourceSets[NPCType].ComboActionDataIndex],
		FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::ComboActionDataLoadCompleted)
	);
}

void AFKCharacterNonPlayer::OnBindResourcesCompleted()
{
	Super::OnBindResourcesCompleted();

	if (HasAuthority())
	{
		DeferredActivate();
	}
}

void AFKCharacterNonPlayer::OnRep_AlreadayResourcesBound()
{
	if (bAlreadayResourcesBound == false)
	{
		return;
	}

	BindCharacterResources();
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
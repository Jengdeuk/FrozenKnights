// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FKCharacterNonPlayer.h"
#include "Engine/AssetManager.h"
#include "Net/UnrealNetwork.h"
#include "Physics/FKCollision.h"
#include "Components/CapsuleComponent.h"
#include "AI/FKAIController.h"
#include "Components/WidgetComponent.h"

AFKCharacterNonPlayer::AFKCharacterNonPlayer()
{
	NPCClass = ENPCClass::Mob;

	// Capsule
	GetCapsuleComponent()->SetCollisionProfileName(CPROFILE_FKMOBCAPSULE);

	GetMesh()->SetHiddenInGame(true);

	AIControllerClass = AFKAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AFKCharacterNonPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void AFKCharacterNonPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	MeshHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(NPCMeshes[uint8(NPCClass)], FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::MeshLoadCompleted));
	AnimHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(NPCAnimInstances[uint8(NPCClass)], FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::AnimLoadCompleted));
	AttackHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(NPCAttackMontages[uint8(NPCClass)], FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::AttackMontageLoadCompleted));
	DeadHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(NPCDeadMontages[uint8(NPCClass)], FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::DeadMontageLoadCompleted));
	ComboActionHandle = UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(NPCComboActionData[uint8(NPCClass)], FStreamableDelegate::CreateUObject(this, &AFKCharacterBase::ComboActionDataLoadCompleted));
}

void AFKCharacterNonPlayer::SetDead()
{
	Super::SetDead();
}

void AFKCharacterNonPlayer::OnInitMeshCompleted()
{
	HpBar->SetHiddenInGame(false);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FKCharacterBase.h"
#include "Interface/FKCharacterAIInterface.h"
#include "FKCharacterNonPlayer.generated.h"

UENUM()
enum class ENPCClass : uint8
{
	Mob,
	Boss
};

UCLASS(config = ProjectFK)
class PROJECTFK_API AFKCharacterNonPlayer : public AFKCharacterBase, public IFKCharacterAIInterface
{
	GENERATED_BODY()
	
public:
	AFKCharacterNonPlayer();

protected:
	virtual void PostInitializeComponents() override;
	virtual void SetDead() override;

public:
	void ActivatePoolableMonster(uint32 InMonsterId, class AFKMonsterPoolManager* InPoolManager);
	void SetHomePos(FVector InPosition);
	virtual void Activate() override;
	virtual void Deactivate() override;

protected:
	ENPCClass NPCClass;

// Resource Section
protected:
	UPROPERTY(config)
	TArray<FSoftObjectPath> Meshes;

	UPROPERTY(config)
	TArray<FSoftObjectPath> AnimInstances;

	UPROPERTY(config)
	TArray<FSoftObjectPath> AttackMontages;

	UPROPERTY(config)
	TArray<FSoftObjectPath> DeadMontages;

	UPROPERTY(config)
	TArray<FSoftObjectPath> ComboActionDatas;

public:
	virtual void OnBindResourcesCompleted() override;

// AI Section
protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;

	FAICharacterAttackFinished OnAttackFinished;

private:
	uint32 MonsterId;
	TWeakObjectPtr<class AFKMonsterPoolManager> PoolManager;
};

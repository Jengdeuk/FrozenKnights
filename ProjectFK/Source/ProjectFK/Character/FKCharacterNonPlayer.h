// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FKCharacterBase.h"
#include "Interface/FKCharacterAIInterface.h"
#include "FKCharacterNonPlayer.generated.h"

UENUM()
enum class ENPCType : uint8
{
	Warchief,
	Qilin,
	BeetleRed,
	Boss
};

USTRUCT()
struct FResourceSet
{
	GENERATED_BODY()

	UPROPERTY()
	uint8 MeshIndex = 0;

	UPROPERTY()
	uint8 AnimInstanceIndex = 0;

	UPROPERTY()
	uint8 StartMontageIndex = 0;

	UPROPERTY()
	uint8 AttackMontageIndex = 0;

	UPROPERTY()
	uint8 DeadMontageIndex = 0;

	UPROPERTY()
	uint8 ComboActionDataIndex = 0;
};

UCLASS(config = ProjectFK)
class PROJECTFK_API AFKCharacterNonPlayer : public AFKCharacterBase, public IFKCharacterAIInterface
{
	GENERATED_BODY()
	
public:
	AFKCharacterNonPlayer();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetDead() override;

public:
	void ActivatePoolableMonster(uint32 InMonsterId, class AFKMonsterPoolManager* InPoolManager);
	void SetType(ENPCType NewType);
	void SetHomePos(FVector InPosition);

	virtual void Activate() override;
	virtual void Deactivate() override;

	bool IsPreparingActivate() const { return bPreparingActivate; }

private:
	bool bPreparingActivate;

protected:
	UPROPERTY(Replicated)
	ENPCType NPCType;

	UPROPERTY(ReplicatedUsing = "OnRep_AlreadayResourcesBound")
	bool bAlreadayResourcesBound;

// Resource Section
protected:
	UPROPERTY(config)
	TArray<FSoftObjectPath> Meshes;

	UPROPERTY(config)
	TArray<FSoftObjectPath> AnimInstances;

	UPROPERTY(config)
	TArray<FSoftObjectPath> StartMontages;

	UPROPERTY(config)
	TArray<FSoftObjectPath> AttackMontages;

	UPROPERTY(config)
	TArray<FSoftObjectPath> DeadMontages;

	UPROPERTY(config)
	TArray<FSoftObjectPath> ComboActionDatas;

	UPROPERTY()
	TMap<ENPCType, FResourceSet> ResourceSets;

public:
	void BindCharacterResources();
	virtual void OnBindResourcesCompleted() override;

	UFUNCTION()
	void OnRep_AlreadayResourcesBound();

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

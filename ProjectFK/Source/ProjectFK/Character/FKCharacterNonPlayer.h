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

UCLASS(config=ProjectFK)
class PROJECTFK_API AFKCharacterNonPlayer : public AFKCharacterBase, public IFKCharacterAIInterface
{
	GENERATED_BODY()
	
public:
	AFKCharacterNonPlayer();

protected:
	virtual void PostInitializeComponents() override;
	virtual void SetDead() override;

protected:
	ENPCClass NPCClass;

public:
	void OnInitMeshCompleted();

protected:
	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCMeshes;

	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCAnimInstances;

	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCAttackMontages;

	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCDeadMontages;

	UPROPERTY(config)
	TArray<FSoftObjectPath> NPCComboActionData;

// AI Section
protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;

	FAICharacterAttackFinished OnAttackFinished;
};

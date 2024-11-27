// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FKCharacterBase.h"
#include "FKCharacterNonPlayer.generated.h"

UENUM()
enum class ENPCClass : uint8
{
	Mob,
	Boss
};

UCLASS(config=ProjectFK)
class PROJECTFK_API AFKCharacterNonPlayer : public AFKCharacterBase
{
	GENERATED_BODY()
	
public:
	AFKCharacterNonPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
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
};

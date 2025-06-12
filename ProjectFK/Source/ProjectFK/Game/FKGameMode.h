// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FKGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFK_API AFKGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AFKGameMode();

public:
	virtual void StartPlay() override;

private:
	TObjectPtr<class AFKMonsterPoolManager> MobPoolManager;
};

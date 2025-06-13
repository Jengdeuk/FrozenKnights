// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "FKAIController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFK_API AFKAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AFKAIController();

public:
	void RunAI();
	void StopAI();
	void SetHomePos(FVector InPosition);

protected:
	virtual void OnPossess(APawn* InPawn) override;

protected:
	UPROPERTY()
	TObjectPtr<class UBlackboardData> BBAsset;

	UPROPERTY()
	TObjectPtr<class UBehaviorTree> BTAsset;
};

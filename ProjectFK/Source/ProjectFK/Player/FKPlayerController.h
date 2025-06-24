// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FKPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFK_API AFKPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AFKPlayerController();

public:
	virtual void BeginPlay() override;
	void PreparingToReturn();

protected:
	FTimerHandle RespawnTimerHandle;

public:
	virtual void StartGame();
	virtual void PauseGame();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCSelectClass(const EPlayerClass& PlayerClass);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UFKClassSelectWidget> ClassSelectUIClass;

	UPROPERTY()
	TObjectPtr<class UFKClassSelectWidget> ClassSelectUI;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UFKRespawnWidget> RespawnUIClass;

	UPROPERTY()
	TObjectPtr<class UFKRespawnWidget> RespawnUI;
};

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

public:
	void StartGame();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCSelectClass(const EPlayerClass& PlayerClass);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UFKClassSelectWidget> ClassSelectUIClass;

	UPROPERTY()
	TObjectPtr<class UFKClassSelectWidget> ClassSelectUI;
};

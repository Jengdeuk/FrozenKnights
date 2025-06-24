// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/FKPlayerController.h"
#include "FKGASPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFKGAS_API AFKGASPlayerController : public AFKPlayerController
{
	GENERATED_BODY()
public:
	AFKGASPlayerController();

public:
	virtual void StartGame() override;
	virtual void PauseGame() override;

public:
	void SetGAS();

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UFKGASHUDWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<class UFKGASHUDWidget> HUDWidget;
};

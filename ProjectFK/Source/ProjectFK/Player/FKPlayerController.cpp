// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FKPlayerController.h"
#include "Character/FKCharacterPlayer.h"

void AFKPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}

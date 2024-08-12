// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FKPlayerController.h"
#include "Character/FKCharacterPlayer.h"
#include "Player/FKPlayerState.h"
#include "UI/FKClassSelectWidget.h"
#include "EngineUtils.h"

AFKPlayerController::AFKPlayerController()
{
	static ConstructorHelpers::FClassFinder<UFKClassSelectWidget> ClassSelectUIClassRef(TEXT("/Game/FrozenKnights/UI/WBP_ClassSelect.WBP_ClassSelect_C"));
	if (ClassSelectUIClassRef.Class)
	{
		ClassSelectUIClass = ClassSelectUIClassRef.Class;
	}

	bReplicates = true;
}

void AFKPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	ClassSelectUI = CreateWidget<UFKClassSelectWidget>(this, ClassSelectUIClass);
	if (ClassSelectUI)
	{
		ClassSelectUI->AddToViewport();
	}

	SetShowMouseCursor(true);
	SetInputMode(FInputModeUIOnly());
}

void AFKPlayerController::StartGame()
{
	SetShowMouseCursor(false);
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}

bool AFKPlayerController::ServerRPCSelectClass_Validate(const EPlayerClass& PlayerClass)
{
	return true;
}

void AFKPlayerController::ServerRPCSelectClass_Implementation(const EPlayerClass& PlayerClass)
{
	AFKPlayerState* PS = GetPlayerState<AFKPlayerState>();
	PS->SetPlayerClass(PlayerClass);
	PS->OnRep_UpdatePlayerClass();
}
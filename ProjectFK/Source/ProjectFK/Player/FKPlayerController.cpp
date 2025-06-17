// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FKPlayerController.h"
#include "Character/FKCharacterPlayer.h"
#include "Player/FKPlayerState.h"
#include "UI/FKClassSelectWidget.h"
#include "UI/FKRespawnWidget.h"
#include "EngineUtils.h"

AFKPlayerController::AFKPlayerController()
{
	bReplicates = true;

	static ConstructorHelpers::FClassFinder<UFKClassSelectWidget> ClassSelectUIClassRef(TEXT("/Game/FrozenKnights/UI/WBP_ClassSelect.WBP_ClassSelect_C"));
	if (ClassSelectUIClassRef.Class)
	{
		ClassSelectUIClass = ClassSelectUIClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<UFKRespawnWidget> RespawnUIClassRef(TEXT("/Game/FrozenKnights/UI/WBP_Respawn.WBP_Respawn_C"));
	if (RespawnUIClassRef.Class)
	{
		RespawnUIClass = RespawnUIClassRef.Class;
	}
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

	RespawnUI = CreateWidget<UFKRespawnWidget>(this, RespawnUIClass);
	if (RespawnUI)
	{
		RespawnUI->SetOwnerCharacter(Cast<AFKCharacterPlayer>(GetPawn()));
		RespawnUI->AddToViewport();
		RespawnUI->SetVisibility(ESlateVisibility::Hidden);
	}

	SetShowMouseCursor(true);
	SetInputMode(FInputModeUIOnly());
}

void AFKPlayerController::PreparingToReturn()
{
	if (RespawnUI)
	{
		RespawnUI->SetVisibility(ESlateVisibility::Visible);
	}
}

void AFKPlayerController::StartGame()
{
	SetShowMouseCursor(false);
	SetInputMode(FInputModeGameOnly());
}

void AFKPlayerController::PauseGame()
{
	SetShowMouseCursor(true);
	SetInputMode(FInputModeUIOnly());
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
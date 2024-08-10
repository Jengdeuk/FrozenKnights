// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FKPlayerState.h"
#include "Character/FKCharacterPlayer.h"
#include "Net/UnrealNetwork.h"

AFKPlayerState::AFKPlayerState()
{
	PlayerClass = EPlayerClass::None;

	bReplicates = true;
}

void AFKPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFKPlayerState, PlayerClass);
}

void AFKPlayerState::OnRep_UpdatePlayerClass()
{
	if (GetOwningController() == nullptr)
	{
		return;
	}

	GetPawn<AFKCharacterPlayer>()->UpdateMeshFromPlayerState();
}

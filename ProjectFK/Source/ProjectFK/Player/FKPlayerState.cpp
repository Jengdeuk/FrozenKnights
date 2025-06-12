// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FKPlayerState.h"
#include "Character/FKCharacterPlayer.h"
#include "Net/UnrealNetwork.h"
#include "ProjectFK.h"

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
	AFKCharacterPlayer* Player = GetPawn<AFKCharacterPlayer>();
	if (!Player)
	{
		return;
	}

	Player->BindCharacterResources();
}
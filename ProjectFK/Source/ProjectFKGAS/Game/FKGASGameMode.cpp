// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FKGASGameMode.h"
#include "FKGASGameMode.h"
#include "Character/FKCharacterNonPlayer.h"

AFKGASGameMode::AFKGASGameMode()
{
	static ConstructorHelpers::FClassFinder<AFKCharacterNonPlayer> MonsterClassRef(TEXT("/Game/FrozenKnightsGAS/Bluprint/BP_FKGASCharacterNonPlayer.BP_FKGASCharacterNonPlayer_C"));
	if (MonsterClassRef.Succeeded())
	{
		MonsterClass = MonsterClassRef.Class;
	}
}
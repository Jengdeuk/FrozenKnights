// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FKGameMode.h"
#include "FKMonsterPoolManager.h"

AFKGameMode::AFKGameMode()
{
	MobPoolManager = CreateDefaultSubobject<AFKMonsterPoolManager>(TEXT("MobPoolManager"));
}

void AFKGameMode::StartPlay()
{
	Super::StartPlay();
}

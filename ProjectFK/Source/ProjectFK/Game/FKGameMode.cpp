// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FKGameMode.h"

AFKGameMode::AFKGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/FrozenKnights/Bluprint/BP_FKCharacterPlayer.BP_FKCharacterPlayer_C'"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/ProjectFK.FKPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	//GameStateClass = AFKGameState::StaticClass();
	//PlayerStateClass = AFKPlayerState::StaticClass();
}

void AFKGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

APlayerController* AFKGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	APlayerController* NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	return NewPlayerController;
}

void AFKGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AFKGameMode::StartPlay()
{
	Super::StartPlay();
}

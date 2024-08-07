// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/FKMenuGameMode.h"

AFKMenuGameMode::AFKMenuGameMode()
{
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/ProjectFK.FKMenuPlayerController"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}
}

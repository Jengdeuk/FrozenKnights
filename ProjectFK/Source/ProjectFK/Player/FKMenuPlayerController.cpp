// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FKMenuPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/FKMainMenuWidget.h"

AFKMenuPlayerController::AFKMenuPlayerController()
{
	static ConstructorHelpers::FClassFinder<UFKMainMenuWidget> MenuUIClassRef(TEXT("/Game/FrozenKnights/UI/WBP_MainMenu.WBP_MainMenu_C"));
	if (MenuUIClassRef.Class)
	{
		MenuUIClass = MenuUIClassRef.Class;
	}
}

void AFKMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	MenuUI = CreateWidget<UFKMainMenuWidget>(this, MenuUIClass);
	if (MenuUI)
	{
		MenuUI->AddToViewport();
	}

	SetShowMouseCursor(true);
	SetInputMode(FInputModeUIOnly());
}

void AFKMenuPlayerController::CreateServer()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("MainLevel"), true, ((FString)(L"Listen")));
}

void AFKMenuPlayerController::JoinServer()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName("127.0.0.1"));
}

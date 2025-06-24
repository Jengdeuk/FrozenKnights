// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FKGASPlayerController.h"
#include "UI/FKGASHUDWidget.h"

AFKGASPlayerController::AFKGASPlayerController()
{
	static ConstructorHelpers::FClassFinder<UFKGASHUDWidget> HUDWidgetClassRef(TEXT("/Game/FrozenKnights/UI/WBP_HUD.WBP_HUD_C"));
	if (HUDWidgetClassRef.Class)
	{
		HUDWidgetClass = HUDWidgetClassRef.Class;
	}
}

void AFKGASPlayerController::StartGame()
{
	Super::StartGame();

	if (HUDWidget)
	{
		HUDWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AFKGASPlayerController::PauseGame()
{
	Super::PauseGame();

	if (HUDWidget)
	{
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AFKGASPlayerController::SetGAS()
{
	if (!IsLocalController())
	{
		return;
	}

	HUDWidget = CreateWidget<UFKGASHUDWidget>(this, HUDWidgetClass);
	if (HUDWidget)
	{
		HUDWidget->SetAbilitySystemComponent(GetPawn());
		HUDWidget->AddToViewport();
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
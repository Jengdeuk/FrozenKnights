// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FKMainMenuWidget.h"
#include "Components/Button.h"
#include "Player/FKMenuPlayerController.h"

void UFKMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CreateButton = Cast<UButton>(GetWidgetFromName(TEXT("BtnCreateServer")));
	ensure(CreateButton);

	JoinButton = Cast<UButton>(GetWidgetFromName(TEXT("BtnJoinServer")));
	ensure(JoinButton);

	CreateButton->OnClicked.AddDynamic(this, &UFKMainMenuWidget::OnCreateServer);
	JoinButton->OnClicked.AddDynamic(this, &UFKMainMenuWidget::OnJoinServer);
}

void UFKMainMenuWidget::OnCreateServer()
{
	AFKMenuPlayerController* MenuController = Cast<AFKMenuPlayerController>(GetOwningPlayer());
	if (MenuController)
	{
		MenuController->CreateServer();
	}
}

void UFKMainMenuWidget::OnJoinServer()
{
	AFKMenuPlayerController* MenuController = Cast<AFKMenuPlayerController>(GetOwningPlayer());
	if (MenuController)
	{
		MenuController->JoinServer();
	}
}

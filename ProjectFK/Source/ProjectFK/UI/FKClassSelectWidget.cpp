// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FKClassSelectWidget.h"
#include "Components/Button.h"
#include "Player/FKPlayerController.h"
#include "Player/FKPlayerState.h"

void UFKClassSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	KnightButton = Cast<UButton>(GetWidgetFromName(TEXT("BtnSelectKnight")));
	ensure(KnightButton);

	MageButton = Cast<UButton>(GetWidgetFromName(TEXT("BtnSelectMage")));
	ensure(MageButton);

	KnightButton->OnClicked.AddDynamic(this, &UFKClassSelectWidget::OnSelectKnight);
	MageButton->OnClicked.AddDynamic(this, &UFKClassSelectWidget::OnSelectMage);
}

void UFKClassSelectWidget::OnSelectKnight()
{
	AFKPlayerController* PC = Cast<AFKPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->ServerRPCSelectClass(EPlayerClass::Knight);
		RemoveFromParent();
	}
}

void UFKClassSelectWidget::OnSelectMage()
{
	AFKPlayerController* PC = Cast<AFKPlayerController>(GetOwningPlayer());
	if (PC)
	{
		PC->ServerRPCSelectClass(EPlayerClass::Mage);
		RemoveFromParent();
	}
}

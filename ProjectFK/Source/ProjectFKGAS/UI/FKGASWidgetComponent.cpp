// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FKGASWidgetComponent.h"
#include "UI/FKGASUserWidget.h"

void UFKGASWidgetComponent::InitWidget()
{
	Super::InitWidget();

	UFKGASUserWidget* GASUserWidget = Cast<UFKGASUserWidget>(GetWidget());
	if (GASUserWidget)
	{
		GASUserWidget->SetAbilitySystemComponent(GetOwner());
	}
}

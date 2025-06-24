// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FKGASHUDWidget.h"
#include "UI/FKGASHpBarUserWidget.h"

void UFKGASHUDWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	WBP_HpBar->SetAbilitySystemComponent(InOwner);
}

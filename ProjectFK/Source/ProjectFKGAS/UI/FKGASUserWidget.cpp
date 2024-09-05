// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FKGASUserWidget.h"
#include "AbilitySystemBlueprintLibrary.h"

void UFKGASUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	if (IsValid(InOwner))
	{
		ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner);
	}
}

UAbilitySystemComponent* UFKGASUserWidget::GetAbilitySystemComponent() const
{
	return ASC;
}

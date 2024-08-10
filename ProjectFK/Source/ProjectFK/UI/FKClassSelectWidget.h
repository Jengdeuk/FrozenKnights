// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FKClassSelectWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFK_API UFKClassSelectWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	virtual void OnSelectKnight();

	UFUNCTION()
	virtual void OnSelectMage();

protected:
	UPROPERTY()
	TObjectPtr<class UButton> KnightButton;

	UPROPERTY()
	TObjectPtr<class UButton> MageButton;
};

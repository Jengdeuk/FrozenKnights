// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FKMainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFK_API UFKMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	virtual void OnCreateServer();
	
	UFUNCTION()
	virtual void OnJoinServer();

protected:
	UPROPERTY()
	TObjectPtr<class UButton> CreateButton;

	UPROPERTY()
	TObjectPtr<class UButton> JoinButton;
};

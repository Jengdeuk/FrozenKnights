// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/FKGASUserWidget.h"
#include "FKGASHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFKGAS_API UFKGASHUDWidget : public UFKGASUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UFKGASHpBarUserWidget> WBP_HpBar;
};

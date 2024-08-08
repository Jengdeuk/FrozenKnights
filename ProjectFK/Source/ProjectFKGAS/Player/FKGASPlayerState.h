// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Player/FKPlayerState.h"
#include "AbilitySystemInterface.h"
#include "FKGASPlayerState.generated.h"

UCLASS()
class PROJECTFKGAS_API AFKGASPlayerState : public AFKPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AFKGASPlayerState();

public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;
};

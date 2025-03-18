// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FKCharacterNonPlayer.h"
#include "AbilitySystemInterface.h"
#include "FKGASCharacterNonPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFKGAS_API AFKGASCharacterNonPlayer : public AFKCharacterNonPlayer, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AFKGASCharacterNonPlayer();

public:
	virtual void PostInitializeComponents() override;
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION()
	virtual void OnOutOfHealth();

protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<class UFKCharacterAttributeSet> AttributeSet;
};

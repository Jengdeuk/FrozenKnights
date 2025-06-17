// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FKCharacterPlayer.h"
#include "AbilitySystemInterface.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "FKGASCharacterPlayer.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFKGAS_API AFKGASCharacterPlayer : public AFKCharacterPlayer, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AFKGASCharacterPlayer();
	
public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void SetupGASInputComponent();
	void GASInputPressed(int32 InputId);
	void GASInputReleased(int32 InputId);

	UFUNCTION()
	virtual void OnOutOfHealth();

	virtual void SetDead() override;
	
public:
	virtual void Activate() override;
	virtual void Deactivate() override;

protected:
	virtual void OnRep_PlayerState() override;

protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

	UPROPERTY(EditAnywhere, Category = GAS)
	TMap<int32, TSubclassOf<class UGameplayAbility>> StartInputAbilities;

private:
	void SetGAS();
};

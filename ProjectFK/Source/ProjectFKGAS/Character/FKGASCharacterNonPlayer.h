// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FKCharacterNonPlayer.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
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

	virtual void SetDead() override;

	void NotifyComboActionEnd();

public:
	virtual void Activate() override;
	virtual void Deactivate() override;

// AI Section
protected:
	virtual float GetAIPatrolRadius() override;
	virtual float GetAIDetectRange() override;
	virtual float GetAIAttackRange() override;
	virtual float GetAITurnSpeed() override;

	virtual void SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished) override;
	virtual void AttackByAI() override;

	FAICharacterAttackFinished OnAttackFinished;

private:
	void OnSpeedChanged(const FOnAttributeChangeData& ChangeData);

	UFUNCTION()
	void OnRepSpeedChanged(float ChangeValue);

protected:
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<class UFKCharacterAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, Category = GAS)
	TArray<TSubclassOf<class UGameplayAbility>> StartAbilities;

private:
	void SetGAS();
};

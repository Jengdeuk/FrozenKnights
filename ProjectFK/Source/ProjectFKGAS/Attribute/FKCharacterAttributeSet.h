// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "FKCharacterAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOutOfHealthDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRepSpeedChangedDelegate, float, ChangeValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRepHealthChangedDelegate, float, ChangeValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRepMaxHealthChangedDelegate, float, ChangeValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRepStaminaChangedDelegate, float, ChangeValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRepMaxStaminaChangedDelegate, float, ChangeValue);

/**
 * 
 */
UCLASS()
class PROJECTFKGAS_API UFKCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UFKCharacterAttributeSet();

	ATTRIBUTE_ACCESSORS(UFKCharacterAttributeSet, AttackRange);
	ATTRIBUTE_ACCESSORS(UFKCharacterAttributeSet, AttackRadius);
	ATTRIBUTE_ACCESSORS(UFKCharacterAttributeSet, AttackRate);
	ATTRIBUTE_ACCESSORS(UFKCharacterAttributeSet, Speed);
	ATTRIBUTE_ACCESSORS(UFKCharacterAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UFKCharacterAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UFKCharacterAttributeSet, Stamina);
	ATTRIBUTE_ACCESSORS(UFKCharacterAttributeSet, MaxStamina);
	ATTRIBUTE_ACCESSORS(UFKCharacterAttributeSet, Damage);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

public:
	void SetRespawn();

	mutable FOutOfHealthDelegate OnOutOfHealth;
	mutable FOnRepSpeedChangedDelegate OnRepSpeedChanged;
	mutable FOnRepHealthChangedDelegate OnRepHealthChanged;
	mutable FOnRepMaxHealthChangedDelegate OnRepMaxHealthChanged;
	mutable FOnRepStaminaChangedDelegate OnRepStaminaChanged;
	mutable FOnRepMaxStaminaChangedDelegate OnRepMaxStaminaChanged;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Replicated, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackRange;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Replicated, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackRadius;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Replicated, Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackRate;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", ReplicatedUsing = "OnRep_SpeedChanged", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Speed;

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = "OnRep_HealthChanged", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = "OnRep_MaxHealthChanged", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = "OnRep_StaminaChanged", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Stamina;

	UPROPERTY(BlueprintReadOnly, Category = "Stamina", ReplicatedUsing = "OnRep_MaxStaminaChanged", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxStamina;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

	UPROPERTY(Replicated)
	bool bOutOfHealth;

private:
	UFUNCTION()
	void OnRep_SpeedChanged();

	UFUNCTION()
	void OnRep_HealthChanged();

	UFUNCTION()
	void OnRep_MaxHealthChanged();

	UFUNCTION()
	void OnRep_StaminaChanged();

	UFUNCTION()
	void OnRep_MaxStaminaChanged();
};

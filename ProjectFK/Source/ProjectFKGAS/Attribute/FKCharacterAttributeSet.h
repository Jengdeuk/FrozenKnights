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

/**
 * 
 */
UCLASS()
class PROJECTFKGAS_API UFKCharacterAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UFKCharacterAttributeSet();

	ATTRIBUTE_ACCESSORS(UFKCharacterAttributeSet, AttackRate);
	ATTRIBUTE_ACCESSORS(UFKCharacterAttributeSet, MaxAttackRate);
	ATTRIBUTE_ACCESSORS(UFKCharacterAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UFKCharacterAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UFKCharacterAttributeSet, Damage);

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	mutable FOutOfHealthDelegate OnOutOfHealth;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = "OnRep_AttackRate", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackRate;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", ReplicatedUsing = "OnRep_MaxAttackRate", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAttackRate;

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = "OnRep_Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, Category = "Health", ReplicatedUsing = "OnRep_MaxHealth", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

	UFUNCTION()
	void OnRep_AttackRate(const FGameplayAttributeData& OldAttackRate) const;

	UFUNCTION()
	void OnRep_MaxAttackRate(const FGameplayAttributeData& OldMaxAttackRate) const;

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	bool bOutOfHealth = false;
};

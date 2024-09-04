// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/FKCharacterAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Tag/FKGameplayTag.h"
#include "Net/UnrealNetwork.h"

UFKCharacterAttributeSet::UFKCharacterAttributeSet() :
	AttackRate(30.0f),
	MaxAttackRate(100.0f),
	MaxHealth(100.0f),
	Damage(0.0f)
{
	InitHealth(GetMaxHealth());
}

void UFKCharacterAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	if (Attribute == GetDamageAttribute())
	{
		NewValue = NewValue < 0.0f ? 0.0f : NewValue;
	}
}

bool UFKCharacterAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude > 0.0f)
		{
			if (Data.Target.HasMatchingGameplayTag(FKTAG_CHARACTER_ISINVINSIBLE))
			{
				Data.EvaluatedData.Magnitude = 0.0f;
				return false;
			}
		}
	}

	return true;
}

void UFKCharacterAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHealth = 0.0f;
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.0f);
	}

	if (GetHealth() <= 0.0f && !bOutOfHealth)
	{
		Data.Target.AddLooseGameplayTag(FKTAG_CHARACTER_ISDEAD);
		OnOutOfHealth.Broadcast();
	}

	bOutOfHealth = (GetHealth() <= 0.0f);
}

void UFKCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFKCharacterAttributeSet, AttackRate);
	DOREPLIFETIME(UFKCharacterAttributeSet, MaxAttackRate);
	DOREPLIFETIME(UFKCharacterAttributeSet, Health);
	DOREPLIFETIME(UFKCharacterAttributeSet, MaxHealth);
}

void UFKCharacterAttributeSet::OnRep_AttackRate(const FGameplayAttributeData& OldAttackRate) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFKCharacterAttributeSet, AttackRate, OldAttackRate);
}

void UFKCharacterAttributeSet::OnRep_MaxAttackRate(const FGameplayAttributeData& OldMaxAttackRate) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFKCharacterAttributeSet, MaxAttackRate, OldMaxAttackRate);
}

void UFKCharacterAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFKCharacterAttributeSet, Health, OldHealth);
}

void UFKCharacterAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UFKCharacterAttributeSet, MaxHealth, OldMaxHealth);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/FKCharacterAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Tag/FKGameplayTag.h"
#include "Net/UnrealNetwork.h"

UFKCharacterAttributeSet::UFKCharacterAttributeSet() :
	AttackRange(100.0f),
	AttackRadius(150.0f),
	AttackRate(30.0f),
	Speed(500.0f),
	MaxHealth(100.0f),
	MaxStamina(100.0f),
	Damage(0.0f)
{
	InitHealth(GetMaxHealth());
	bOutOfHealth = false;
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
		Data.Target.ForceReplication();
		OnOutOfHealth.Broadcast();
	}

	bOutOfHealth = (GetHealth() <= 0.0f);
}

void UFKCharacterAttributeSet::SetRespawn()
{
	bOutOfHealth = false;
	SetHealth(GetMaxHealth());
}

void UFKCharacterAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFKCharacterAttributeSet, AttackRange);
	DOREPLIFETIME(UFKCharacterAttributeSet, AttackRadius);
	DOREPLIFETIME(UFKCharacterAttributeSet, AttackRate);
	DOREPLIFETIME(UFKCharacterAttributeSet, Speed);
	DOREPLIFETIME(UFKCharacterAttributeSet, Health);
	DOREPLIFETIME(UFKCharacterAttributeSet, MaxHealth);
	DOREPLIFETIME(UFKCharacterAttributeSet, Stamina);
	DOREPLIFETIME(UFKCharacterAttributeSet, MaxStamina);
	DOREPLIFETIME(UFKCharacterAttributeSet, bOutOfHealth);
}

void UFKCharacterAttributeSet::OnRep_SpeedChanged()
{
	OnRepSpeedChanged.Broadcast(GetSpeed());
}

void UFKCharacterAttributeSet::OnRep_HealthChanged()
{
	OnRepHealthChanged.Broadcast(GetHealth());
}

void UFKCharacterAttributeSet::OnRep_MaxHealthChanged()
{
	OnRepMaxHealthChanged.Broadcast(GetMaxHealth());
}

void UFKCharacterAttributeSet::OnRep_StaminaChanged()
{
	OnRepStaminaChanged.Broadcast(GetStamina());
}

void UFKCharacterAttributeSet::OnRep_MaxStaminaChanged()
{
	OnRepMaxStaminaChanged.Broadcast(GetMaxStamina());
}

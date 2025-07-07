// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FKGASPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Attribute/FKCharacterAttributeSet.h"

AFKGASPlayerState::AFKGASPlayerState()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	AttributeSet = CreateDefaultSubobject<UFKCharacterAttributeSet>(TEXT("AttributeSet"));
}

void AFKGASPlayerState::BeginPlay()
{
	Super::BeginPlay();

	ResetHealth();
}

void AFKGASPlayerState::ResetHealth()
{
	AttributeSet->SetRespawn();
}

void AFKGASPlayerState::SetPlayerClass(const EPlayerClass& InPlayerClass)
{
	Super::SetPlayerClass(InPlayerClass);

	switch (PlayerClass)
	{
	case EPlayerClass::Knight:
		AttributeSet->SetMaxHealth(10000.0f);
		AttributeSet->SetAttackRate(30.0f);
		AttributeSet->SetAttackRadius(200.0f);
		break;
	case EPlayerClass::Mage:
		AttributeSet->SetMaxHealth(4000.0f);
		AttributeSet->SetAttackRate(50.0f);
		break;
	}

	ResetHealth();
}

UAbilitySystemComponent* AFKGASPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

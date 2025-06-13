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

	AttributeSet->SetMaxHealth(10000.0f);
	AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
}

UAbilitySystemComponent* AFKGASPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

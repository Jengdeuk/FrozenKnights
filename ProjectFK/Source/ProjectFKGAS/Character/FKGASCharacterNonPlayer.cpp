// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FKGASCharacterNonPlayer.h"
#include "AbilitySystemComponent.h"
#include "Attribute/FKCharacterAttributeSet.h"
#include "UI/FKGASWidgetComponent.h"
#include "UI/FKGASUserWidget.h"
#include "UI/FKGASHpBarUserWidget.h"

AFKGASCharacterNonPlayer::AFKGASCharacterNonPlayer()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	AttributeSet = CreateDefaultSubobject<UFKCharacterAttributeSet>(TEXT("AttributeSet"));

	// UI
	HpBar = CreateDefaultSubobject<UFKGASWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 215.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/FrozenKnights/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(140.0f, 18.f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HpBar->SetHiddenInGame(true);
	}
}

void AFKGASCharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ASC->InitAbilityActorInfo(this, this);
	AttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
}

UAbilitySystemComponent* AFKGASCharacterNonPlayer::GetAbilitySystemComponent() const
{
	return ASC;
}

void AFKGASCharacterNonPlayer::OnOutOfHealth()
{
	SetDead();
	bDead = true;
}

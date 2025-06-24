// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FKGASHpBarUserWidget.h"
#include "AbilitySystemComponent.h"
#include "Attribute/FKCharacterAttributeSet.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Tag/FKGameplayTag.h"
#include "Character/FKCharacterBase.h"

void UFKGASHpBarUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UFKCharacterAttributeSet::GetHealthAttribute()).AddUObject(this, &UFKGASHpBarUserWidget::OnHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UFKCharacterAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UFKGASHpBarUserWidget::OnMaxHealthChanged);
		ASC->RegisterGameplayTagEvent(FKTAG_CHARACTER_ISINVINSIBLE, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UFKGASHpBarUserWidget::OnInvinsibleTagChanged);
		
		AFKCharacterBase* OwnerCharacter = Cast<AFKCharacterBase>(InOwner);
		PbHpBar->SetFillColorAndOpacity(OwnerCharacter->IsLocallyControlled() == false && OwnerCharacter->IsPlayerCharacter() ? OtherPlayerHealthColor : HealthColor);

		const UFKCharacterAttributeSet* CurrentAttributeSet = ASC->GetSet<UFKCharacterAttributeSet>();
		if (CurrentAttributeSet)
		{
			CurrentHealth = CurrentAttributeSet->GetHealth();
			CurrentMaxHealth = CurrentAttributeSet->GetMaxHealth();

			if (CurrentMaxHealth > 0.0f)
			{
				UpdateHpBar();
			}
		}
	}
}

void UFKGASHpBarUserWidget::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentHealth = ChangeData.NewValue;
	UpdateHpBar();
}

void UFKGASHpBarUserWidget::OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxHealth = ChangeData.NewValue;
	UpdateHpBar();
}

void UFKGASHpBarUserWidget::OnInvinsibleTagChanged(const FGameplayTag CallBackTag, int32 NewCount)
{
	if (NewCount > 0)
	{
		PbHpBar->SetFillColorAndOpacity(InvinsibleColor);
		PbHpBar->SetPercent(1.0f);
	}
	else
	{
		PbHpBar->SetFillColorAndOpacity(HealthColor);
		UpdateHpBar();
	}
}

void UFKGASHpBarUserWidget::UpdateHpBar()
{
	if (PbHpBar)
	{
		PbHpBar->SetPercent(CurrentHealth / CurrentMaxHealth);
	}

	if (TxtHpStat)
	{
		TxtHpStat->SetText(FText::FromString(FString::Printf(TEXT("%.0f/%.0f"), CurrentHealth, CurrentMaxHealth)));
	}
}

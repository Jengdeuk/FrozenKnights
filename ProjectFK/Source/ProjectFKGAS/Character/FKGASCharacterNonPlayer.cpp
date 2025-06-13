// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FKGASCharacterNonPlayer.h"
#include "AbilitySystemComponent.h"
#include "Attribute/FKCharacterAttributeSet.h"
#include "AI/FKGASAIController.h"
#include "UI/FKGASWidgetComponent.h"
#include "UI/FKGASUserWidget.h"
#include "UI/FKGASHpBarUserWidget.h"
#include "GA/FKGA_Attack.h"
#include "Tag/FKGameplayTag.h"

AFKGASCharacterNonPlayer::AFKGASCharacterNonPlayer()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
	AttributeSet = CreateDefaultSubobject<UFKCharacterAttributeSet>(TEXT("AttributeSet"));

	AIControllerClass = AFKGASAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// UI
	HpBar = CreateDefaultSubobject<UFKGASWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 245.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/FrozenKnights/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(75.0f, 10.f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HpBar->SetHiddenInGame(true);
	}
}

void AFKGASCharacterNonPlayer::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SetGAS();
}

UAbilitySystemComponent* AFKGASCharacterNonPlayer::GetAbilitySystemComponent() const
{
	return ASC;
}

void AFKGASCharacterNonPlayer::OnOutOfHealth() // Server에서만 동작
{
	SetDead();
	bDead = true; // 클라로 죽은 상태 동기화
}

void AFKGASCharacterNonPlayer::NotifyComboActionEnd()
{
	OnAttackFinished.ExecuteIfBound();
}

void AFKGASCharacterNonPlayer::Activate()
{
	Super::Activate();

	if (HasAuthority())
	{
		bDead = false;
		ASC->RemoveLooseGameplayTag(FKTAG_CHARACTER_ISDEAD);
		AttributeSet->SetHealth(AttributeSet->GetMaxHealth());
	}
}

void AFKGASCharacterNonPlayer::Deactivate()
{
	Super::Deactivate();
}

float AFKGASCharacterNonPlayer::GetAIPatrolRadius()
{
	return 800.0f;
}

float AFKGASCharacterNonPlayer::GetAIDetectRange()
{
	return 800.0f;
}

float AFKGASCharacterNonPlayer::GetAIAttackRange()
{
	return ASC->GetSet<UFKCharacterAttributeSet>()->GetAttackRange() + ASC->GetSet<UFKCharacterAttributeSet>()->GetAttackRadius();
}

float AFKGASCharacterNonPlayer::GetAITurnSpeed()
{
	return 2.0f;
}

void AFKGASCharacterNonPlayer::SetAIAttackDelegate(const FAICharacterAttackFinished& InOnAttackFinished)
{
	OnAttackFinished = InOnAttackFinished;
}

void AFKGASCharacterNonPlayer::AttackByAI()
{
	if (StartAbilities.Num() > 0 && StartAbilities[0])
	{
		TSubclassOf<UGameplayAbility> AbilityClass = StartAbilities[0];
		FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(AbilityClass);

		if (Spec)
		{
			if (Spec->IsActive())
			{
				ASC->AbilitySpecInputPressed(*Spec);
			}
			else
			{
				ASC->TryActivateAbility(Spec->Handle);
			}
		}
	}
}

void AFKGASCharacterNonPlayer::SetGAS()
{
	ASC->InitAbilityActorInfo(this, this);
	for (const auto& StartAbility : StartAbilities)
	{
		FGameplayAbilitySpec StartSpec(StartAbility);
		ASC->GiveAbility(StartSpec);
	}

	AttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
}

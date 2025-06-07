// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/FKGASCharacterPlayer.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "Player/FKGASPlayerState.h"
#include "ProjectFKGAS.h"
#include "Attribute/FKCharacterAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"
#include "ProjectFKGAS.h"
#include "UI/FKGASWidgetComponent.h"
#include "UI/FKGASUserWidget.h"
#include "UI/FKGASHpBarUserWidget.h"

AFKGASCharacterPlayer::AFKGASCharacterPlayer()
{
	ASC = nullptr;

	// UI
	HpBar = CreateDefaultSubobject<UFKGASWidgetComponent>(TEXT("Widget"));
	HpBar->SetupAttachment(GetMesh());
	HpBar->SetRelativeLocation(FVector(0.0f, 0.0f, 215.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> HpBarWidgetRef(TEXT("/Game/FrozenKnights/UI/WBP_HpBar.WBP_HpBar_C"));
	if (HpBarWidgetRef.Class)
	{
		HpBar->SetWidgetClass(HpBarWidgetRef.Class);
		HpBar->SetWidgetSpace(EWidgetSpace::Screen);
		HpBar->SetDrawSize(FVector2D(140.0f, 10.f));
		HpBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HpBar->SetHiddenInGame(true);
	}
}

UAbilitySystemComponent* AFKGASCharacterPlayer::GetAbilitySystemComponent() const
{
	return ASC;
}

void AFKGASCharacterPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void AFKGASCharacterPlayer::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	SetGAS();
}

void AFKGASCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	SetupGASInputComponent();
}

void AFKGASCharacterPlayer::SetupGASInputComponent()
{
	if (IsValid(ASC) && IsValid(InputComponent))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AFKGASCharacterPlayer::GASInputPressed, 0);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AFKGASCharacterPlayer::GASInputReleased, 0);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AFKGASCharacterPlayer::GASInputPressed, 1);
	}
}

void AFKGASCharacterPlayer::GASInputPressed(int32 InputId)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = true;
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

void AFKGASCharacterPlayer::GASInputReleased(int32 InputId)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = false;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*Spec);
		}
	}
}

void AFKGASCharacterPlayer::OnOutOfHealth()
{
	SetDead();
	bDead = true;
}

void AFKGASCharacterPlayer::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	SetGAS();
}

void AFKGASCharacterPlayer::SetGAS()
{
	AFKGASPlayerState* GASPS = GetPlayerState<AFKGASPlayerState>();
	if (GASPS)
	{
		ASC = GASPS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(GASPS, this);

		const UFKCharacterAttributeSet* CurrentAttributeSet = ASC->GetSet<UFKCharacterAttributeSet>();
		if (CurrentAttributeSet)
		{
			CurrentAttributeSet->OnOutOfHealth.AddDynamic(this, &ThisClass::OnOutOfHealth);
		}

		for (const auto& StartAbility : StartAbilities)
		{
			FGameplayAbilitySpec StartSpec(StartAbility);
			ASC->GiveAbility(StartSpec);
		}

		for (const auto& StartInputAbility : StartInputAbilities)
		{
			FGameplayAbilitySpec StartSpec(StartInputAbility.Value);
			StartSpec.InputID = StartInputAbility.Key;
			ASC->GiveAbility(StartSpec);
		}

		SetupGASInputComponent();

		if (UUserWidget* Widget = HpBar->GetWidget())
		{
			if (UFKGASHpBarUserWidget* HpWidget = Cast<UFKGASHpBarUserWidget>(Widget))
			{
				HpWidget->SetAbilitySystemComponent(this);
			}
		}

		if (IsLocallyControlled())
		{
			APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
			PlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
		}
	}
}

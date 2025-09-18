// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/FKGA_Attack.h"
#include "Character/FKCharacterBase.h"
#include "Character/FKCharacterPlayer.h"
#include "Character/FKGASCharacterPlayer.h"
#include "Character/FKGASCharacterNonPlayer.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/FKComboActionData.h"

UFKGA_Attack::UFKGA_Attack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UFKGA_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AFKCharacterBase* FKCharacter = CastChecked<AFKCharacterBase>(ActorInfo->AvatarActor.Get());
	CurrentComboData = FKCharacter->GetComboActionData();
	bPlayerCharacter = FKCharacter->IsPlayerCharacter();
	if (bPlayerCharacter)
	{
		FKCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}

	UAbilityTask_PlayMontageAndWait* PlayAttackTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayAttack"), FKCharacter->GetAttackMontage(), 1.0f, GetNextSection());
	PlayAttackTask->OnCompleted.AddDynamic(this, &UFKGA_Attack::OnCompleteCallback);
	PlayAttackTask->OnInterrupted.AddDynamic(this, &UFKGA_Attack::OnInterruptedCallback);
	PlayAttackTask->ReadyForActivation();

	if (bPlayerCharacter)
	{
		AFKCharacterPlayer* FKPlayer = CastChecked<AFKCharacterPlayer>(ActorInfo->AvatarActor.Get());

		FKPlayer->ServerRPCPlayAttackMontage();
		if (FKPlayer->IsLocallyControlled())
		{
			FKPlayer->CalculateClientAimPoint();
		}
	}

	StartComboTimer();
}

void UFKGA_Attack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (!ComboTimerHandle.IsValid())
	{
		bHasNextComboInput = false;
	}
	else
	{
		bHasNextComboInput = true;
	}
}

void UFKGA_Attack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UFKGA_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (bPlayerCharacter)
	{
		AFKCharacterBase* FKCharacter = CastChecked<AFKCharacterBase>(ActorInfo->AvatarActor.Get());
		FKCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	else
	{
		AFKGASCharacterNonPlayer* FKCharacter = CastChecked<AFKGASCharacterNonPlayer>(ActorInfo->AvatarActor.Get());
		FKCharacter->NotifyComboActionEnd();
	}

	CurrentComboData = nullptr;
	CurrentCombo = 0;
	bHasNextComboInput = false;
}

void UFKGA_Attack::OnCompleteCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UFKGA_Attack::OnInterruptedCallback()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

FName UFKGA_Attack::GetNextSection()
{
	CurrentCombo = FMath::Clamp(CurrentCombo + 1, 1, CurrentComboData->MaxComboCount);
	FName NextSection = *FString::Printf(TEXT("%s%d"), *CurrentComboData->MontageSectionNamePrefix, CurrentCombo);
	return NextSection;
}

void UFKGA_Attack::StartComboTimer()
{
	int32 ComboIndex = CurrentCombo - 1;
	ensure(CurrentComboData->EffectiveFrameCount.IsValidIndex(ComboIndex));

	float ComboEffectiveTime = CurrentComboData->EffectiveFrameCount[ComboIndex] / CurrentComboData->FrameRate;
	if (ComboEffectiveTime > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UFKGA_Attack::CheckComboInput, ComboEffectiveTime, false);
	}
}

void UFKGA_Attack::CheckComboInput()
{
	ComboTimerHandle.Invalidate();
	if (bHasNextComboInput)
	{
		FName NextSectionName = GetNextSection();
		MontageJumpToSection(NextSectionName);

		if (bPlayerCharacter)
		{
			AFKCharacterPlayer* FKCharacter = CastChecked<AFKCharacterPlayer>(GetActorInfo().AvatarActor.Get());
			if (FKCharacter)
			{
				FKCharacter->ServerRPCMontageJumpToSection(NextSectionName);
				if (FKCharacter->IsLocallyControlled())
				{
					FKCharacter->CalculateClientAimPoint();
				}
			}
		}

		StartComboTimer();
		bHasNextComboInput = false;
	}
}
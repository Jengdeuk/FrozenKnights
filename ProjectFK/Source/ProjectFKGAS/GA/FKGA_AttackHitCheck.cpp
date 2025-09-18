// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/FKGA_AttackHitCheck.h"
#include "AT/FKAT_Trace.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Tag/FKGameplayTag.h"
#include "AbilitySystemComponent.h"
#include "Character/FKCharacterBase.h"
#include "Player/FKGASPlayerState.h"
#include "Components/CapsuleComponent.h"

UFKGA_AttackHitCheck::UFKGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UFKGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentLevel = TriggerEventData->EventMagnitude;

	UFKAT_Trace* AttackTraceTask = UFKAT_Trace::CreateTask(this, TargetActorClass);
	AttackTraceTask->OnComplete.AddDynamic(this, &UFKGA_AttackHitCheck::OnTraceResultCallback);
	AttackTraceTask->ReadyForActivation();

	// 소환 이펙트 소환
	if (UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked())
	{
		AFKCharacterBase* Character = Cast<AFKCharacterBase>(SourceASC->GetAvatarActor());
		if (Character && Character->IsPlayerCharacter())
		{
			AFKGASPlayerState* GASPS = Cast<AFKGASPlayerState>(SourceASC->GetOwnerActor());
			if (GASPS && GASPS->GetPlayerClass() == EPlayerClass::Mage)
			{
				FGameplayCueParameters CueParam;
				CueParam.Location =
					Character->GetActorLocation()
					+ Character->GetActorForwardVector() * 2.f * Character->GetCapsuleComponent()->GetScaledCapsuleRadius()
					+ FVector(0, 0, 40);
				SourceASC->ExecuteGameplayCue(GAMEPLAYCUE_CHARACTER_MUZZLEFLASH_MAGE, CueParam);
			}
		}
	}
}

void UFKGA_AttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
	AFKCharacterBase* SourceCharacter = Cast<AFKCharacterBase>(SourceASC->GetAvatarActor());
	if (!SourceASC || !SourceCharacter)
	{
		return;
	}

	bool bIsMonster = (SourceCharacter->IsPlayerCharacter() == false);

	for (int32 i = 0; i < TargetDataHandle.Num(); ++i)
	{
		if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, i))
		{
			FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);

			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
			if (!TargetASC)
			{
				return;
			}

			AFKCharacterBase* TargetCharacter = Cast<AFKCharacterBase>(HitResult.GetActor());
			if (bIsMonster && TargetCharacter->IsPlayerCharacter() == false)
			{
				return;
			}

			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
			if (EffectSpecHandle.IsValid())
			{
				// 대미지 주기
				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

				// 피격 이펙트 소환
				FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
				CueContextHandle.AddHitResult(HitResult);

				FGameplayCueParameters CueParam;
				CueParam.EffectContext = CueContextHandle;

				FGameplayTag Tag;
				if (AFKGASPlayerState* GASPS = Cast<AFKGASPlayerState>(SourceASC->GetOwnerActor()))
				{
					switch (GASPS->GetPlayerClass())
					{
					case EPlayerClass::Knight:
						Tag = GAMEPLAYCUE_CHARACTER_ATTACKHIT_KNIGHT;
						break;
					case EPlayerClass::Mage:
						Tag = GAMEPLAYCUE_CHARACTER_ATTACKHIT_MAGE;
						break;
					}
				}

				TargetASC->ExecuteGameplayCue(Tag, CueParam);
			}
		}
		else if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, i))
		{
			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
			if (EffectSpecHandle.IsValid())
			{
				// 대미지 주기
				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);

				// 피격 이펙트 소환
				FGameplayEffectContextHandle CueContextHandle = UAbilitySystemBlueprintLibrary::GetEffectContext(EffectSpecHandle);
				CueContextHandle.AddActors(TargetDataHandle.Data[i].Get()->GetActors(), false);

				FGameplayCueParameters CueParam;
				CueParam.EffectContext = CueContextHandle;

				FGameplayTag Tag;
				if (AFKGASPlayerState* GASPS = Cast<AFKGASPlayerState>(SourceASC->GetOwnerActor()))
				{
					switch (GASPS->GetPlayerClass())
					{
					case EPlayerClass::Knight:
						Tag = GAMEPLAYCUE_CHARACTER_ATTACKHIT_KNIGHT;
						break;
					case EPlayerClass::Mage:
						Tag = GAMEPLAYCUE_CHARACTER_ATTACKHIT_MAGE;
						break;
					}
				}

				SourceASC->ExecuteGameplayCue(Tag, CueParam);
			}
		}
	}

	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/FKGA_AttackHitCheck.h"
#include "AT/FKAT_Trace.h"
#include "AbilitySystemBlueprintLibrary.h"

UFKGA_AttackHitCheck::UFKGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UFKGA_AttackHitCheck::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentLevel = TriggerEventData->EventMagnitude;

	UFKAT_Trace* AttackTraceTask = UFKAT_Trace::CreateTask(this, TargetActorClass);
	AttackTraceTask->OnComplete.AddDynamic(this, &UFKGA_AttackHitCheck::OnTraceResultCallback);
	AttackTraceTask->ReadyForActivation();
}

void UFKGA_AttackHitCheck::OnTraceResultCallback(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	for (int32 i = 0; i < TargetDataHandle.Num(); ++i)
	{
		if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, i))
		{
			FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);

			UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
			if (!SourceASC || !TargetASC)
			{
				return;
			}

			FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingGameplayEffectSpec(AttackDamageEffect, CurrentLevel);
			if (EffectSpecHandle.IsValid())
			{
				ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, EffectSpecHandle, TargetDataHandle);
			}
		}
	}

	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

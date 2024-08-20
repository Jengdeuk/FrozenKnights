// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/FKGA_AttackHitCheck.h"
#include "AT/FKAT_Trace.h"
#include "AbilitySystemBlueprintLibrary.h"

UFKGA_AttackHitCheck::UFKGA_AttackHitCheck()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
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
	if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetDataHandle, 0))
	{
		FHitResult HitResult = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetDataHandle, 0);

		UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo_Checked();
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitResult.GetActor());
		if (!SourceASC || !TargetASC)
		{
			return;
		}
	}
	else if (UAbilitySystemBlueprintLibrary::TargetDataHasActor(TargetDataHandle, 0))
	{
	}

	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

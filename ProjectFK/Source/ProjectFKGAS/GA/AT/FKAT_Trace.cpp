// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/FKAT_Trace.h"
#include "GA/TA/FKTA_Trace.h"
#include "AbilitySystemComponent.h"

UFKAT_Trace::UFKAT_Trace()
{
}

UFKAT_Trace* UFKAT_Trace::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<class AGameplayAbilityTargetActor> TargetActorClass)
{
	UFKAT_Trace* NewTask = NewAbilityTask<UFKAT_Trace>(OwningAbility);
	NewTask->TargetActorClass = TargetActorClass;
	return NewTask;
}

void UFKAT_Trace::Activate()
{
	Super::Activate();

	SpawnAndInitializeTargetActor();
	FinalizeTargetActor();

	SetWaitingOnAvatar();
}

void UFKAT_Trace::OnDestroy(bool AbilityEnded)
{
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->Destroy();
	}

	Super::OnDestroy(AbilityEnded);
}

void UFKAT_Trace::SpawnAndInitializeTargetActor()
{
	SpawnedTargetActor = Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(
		TargetActorClass,
		FTransform::Identity,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->TargetDataReadyDelegate.AddUObject(this, &UFKAT_Trace::OnTargetDataReadyCallback);
	}
}

void UFKAT_Trace::FinalizeTargetActor()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC)
	{
		const FTransform SpawnTransform = ASC->GetAvatarActor()->GetTransform();
		SpawnedTargetActor->FinishSpawning(SpawnTransform);

		ASC->SpawnedTargetActors.Push(SpawnedTargetActor);
		SpawnedTargetActor->StartTargeting(Ability);
	}
}

void UFKAT_Trace::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast(DataHandle);
	}

	EndTask();
}

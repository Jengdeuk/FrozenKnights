// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/AT/FKAT_Jump.h"
#include "GameFramework/Character.h"

UFKAT_Jump::UFKAT_Jump()
{
}

UFKAT_Jump* UFKAT_Jump::CreateTask(UGameplayAbility* OwningAbility)
{
	UFKAT_Jump* NewTask = NewAbilityTask<UFKAT_Jump>(OwningAbility);
	return NewTask;
}

void UFKAT_Jump::Activate()
{
	Super::Activate();

	ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());
	Character->LandedDelegate.AddDynamic(this, &UFKAT_Jump::OnLandedCallback);
	Character->Jump();

	SetWaitingOnAvatar();
}

void UFKAT_Jump::OnDestroy(bool AbilityEnded)
{
	ACharacter* Character = CastChecked<ACharacter>(GetAvatarActor());
	Character->LandedDelegate.RemoveDynamic(this, &UFKAT_Jump::OnLandedCallback);

	Super::OnDestroy(AbilityEnded);
}

void UFKAT_Jump::OnLandedCallback(const FHitResult& Hit)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast();
	}
}

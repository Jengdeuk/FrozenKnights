// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FKGASAnimInstance.h"
#include "Character/FKCharacterBase.h"
#include "Character/FKGASCharacterPlayer.h"
#include "Character/FKGASCharacterNonPlayer.h"
#include "Tag/FKGameplayTag.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UFKGASAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (Movement)
	{
		UAbilitySystemComponent* ASC;
		if (!bIsFalling && Movement->IsFalling())
		{
			bIsFalling = true;
			if (CastChecked<AFKCharacterBase>(Owner)->IsPlayerCharacter())
			{
				ASC = CastChecked<AFKGASCharacterPlayer>(Owner)->GetAbilitySystemComponent();
				if (ASC)
				{
					ASC->AddLooseGameplayTag(FKTAG_CHARACTER_ISFALLING);
				}
			}
			else
			{
			}
		}
		else if (bIsFalling && !Movement->IsFalling())
		{
			bIsFalling = false;
			if (CastChecked<AFKCharacterBase>(Owner)->IsPlayerCharacter())
			{
				ASC = CastChecked<AFKGASCharacterPlayer>(Owner)->GetAbilitySystemComponent();
				if (ASC)
				{
					ASC->RemoveLooseGameplayTag(FKTAG_CHARACTER_ISFALLING);
				}
			}
			else
			{
			}
		}
	}

	Super::NativeUpdateAnimation(DeltaSeconds);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/TA/FKTA_Trace.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Physics/FKCollision.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Attribute/FKCharacterAttributeSet.h"
#include "Character/FKCharacterBase.h"

AFKTA_Trace::AFKTA_Trace()
{
	bHasConfirmed = false;
	bShowDebug = false;
}

void AFKTA_Trace::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

void AFKTA_Trace::ConfirmTargeting()
{
	Super::ConfirmTargeting();

	bHasConfirmed = true;
}
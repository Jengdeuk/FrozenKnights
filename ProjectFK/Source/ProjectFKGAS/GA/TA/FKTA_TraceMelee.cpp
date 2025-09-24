// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/TA/FKTA_TraceMelee.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Physics/FKCollision.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Attribute/FKCharacterAttributeSet.h"
#include "Character/FKCharacterPlayer.h"

AFKTA_TraceMelee::AFKTA_TraceMelee()
{
}

void AFKTA_TraceMelee::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	ConfirmTargeting();
}

void AFKTA_TraceMelee::ConfirmTargetingAndContinue()
{
	if (SourceActor)
	{
		FGameplayAbilityTargetDataHandle DataHandle = MakeTargetData();
		TargetDataReadyDelegate.Broadcast(DataHandle);
	}
}

FGameplayAbilityTargetDataHandle AFKTA_TraceMelee::MakeTargetData() const
{
	ACharacter* Character = CastChecked<ACharacter>(SourceActor);

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
	if (!ASC)
	{
		return FGameplayAbilityTargetDataHandle();
	}

	const UFKCharacterAttributeSet* AttributeSet = ASC->GetSet<UFKCharacterAttributeSet>();
	if (!AttributeSet)
	{
		return FGameplayAbilityTargetDataHandle();
	}

	TArray<FHitResult> OutHitResults;
	const float AttackRange = AttributeSet->GetAttackRange();
	const float AttackRadius = AttributeSet->GetAttackRadius();

	FCollisionQueryParams Params(SCENE_QUERY_STAT(UFKTA_Trace), false, Character);
	const FVector Forward = Character->GetActorForwardVector();
	const FVector Start = Character->GetActorLocation() + Forward * Character->GetCapsuleComponent()->GetScaledCapsuleRadius();

	FVector FireDirection = Forward;
	if (Cast<AFKCharacterBase>(Character)->IsPlayerCharacter())
	{
		FireDirection = (Cast<AFKCharacterPlayer>(Character)->GetAimPoint() - Start).GetSafeNormal();
	}
	const FVector End = Start + FireDirection * AttackRange;

	ECollisionChannel ColChannel = CCHANNEL_FKACTION;
	bool HitDetected = GetWorld()->SweepMultiByChannel(OutHitResults, Start, End, FQuat::Identity, ColChannel, FCollisionShape::MakeSphere(AttackRadius), Params);

	FGameplayAbilityTargetDataHandle DataHandle;
	if (HitDetected)
	{
		for (const FHitResult& OutHitResult : OutHitResults)
		{
			FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(OutHitResult);
			DataHandle.Add(TargetData);
		}
	}

#if ENABLE_DRAW_DEBUG

	if (bShowDebug)
	{
		FVector CapsuleOrigin = Start + (End - Start) * 0.5f;
		float CapsuleHalfHeight = AttackRange * 0.5f;
		FColor DrawColor = HitDetected ? FColor::Green : FColor::Red;
		DrawDebugCapsule(
			GetWorld(),
			CapsuleOrigin,
			CapsuleHalfHeight,
			AttackRadius,
			FRotationMatrix::MakeFromZ(Forward).ToQuat(),
			DrawColor,
			false,
			1.0f
		);
	}

#endif

	return DataHandle;
}

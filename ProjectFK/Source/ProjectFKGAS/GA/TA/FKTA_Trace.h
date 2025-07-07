// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "FKTA_Trace.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFKGAS_API AFKTA_Trace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	AFKTA_Trace();

public:
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargeting() override;
	void SetShowDebug(bool InShowDebug) { bShowDebug = InShowDebug; }

protected:
	bool bHasConfirmed;
	bool bShowDebug;
};

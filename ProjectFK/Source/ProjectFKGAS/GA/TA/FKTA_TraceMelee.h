// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/TA/FKTA_Trace.h"
#include "FKTA_TraceMelee.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFKGAS_API AFKTA_TraceMelee : public AFKTA_Trace
{
	GENERATED_BODY()

public:
	AFKTA_TraceMelee();

public:
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;

private:
	FGameplayAbilityTargetDataHandle MakeTargetData() const;
};

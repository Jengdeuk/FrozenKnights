// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/FKAnimInstance.h"
#include "FKGASAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFKGAS_API UFKGASAnimInstance : public UFKAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};

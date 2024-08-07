// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "FKAT_Jump.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FJumpDelegate);

/**
 * 
 */
UCLASS()
class PROJECTFKGAS_API UFKAT_Jump : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFKAT_Jump();

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "Jump", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UFKAT_Jump* CreateTask(UGameplayAbility* OwningAbility);

	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityEnded) override;

public:
	UPROPERTY(BlueprintAssignable)
	FJumpDelegate OnComplete;

protected:
	UFUNCTION()
	void OnLandedCallback(const FHitResult& Hit);
};

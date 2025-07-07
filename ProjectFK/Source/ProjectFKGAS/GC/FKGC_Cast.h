// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "FKGC_Cast.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFKGAS_API UFKGC_Cast : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	UFKGC_Cast();

public:
	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameplayCue)
	TObjectPtr<class UParticleSystem> ParticleSystem;
};

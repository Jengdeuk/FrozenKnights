// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FKFXActor_Projectile.generated.h"

UCLASS()
class PROJECTFKGAS_API AFKFXActor_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AFKFXActor_Projectile();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UParticleSystem> ParticleSystem;
};

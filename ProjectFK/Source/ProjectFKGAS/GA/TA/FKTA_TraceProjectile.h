// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GA/TA/FKTA_Trace.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "FKTA_TraceProjectile.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFKGAS_API AFKTA_TraceProjectile : public AFKTA_Trace
{
	GENERATED_BODY()

public:
	AFKTA_TraceProjectile();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

public:
	virtual void StartTargeting(UGameplayAbility* Ability) override;

	UFUNCTION()
	void OnTimerFinished();
	virtual void ConfirmTargetingAndContinue() override;

protected:
	UFUNCTION()
	void OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnProjectileHitWall(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* CollisionComponent;

	UPROPERTY()
	TObjectPtr<class AFKFXActor_Projectile> FXActor;

private:
	FGameplayAbilityTargetDataHandle DataHandle;
	FTimerHandle ConfirmTimerHandle;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "FKTA_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFKGAS_API AFKTA_Projectile : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AFKTA_Projectile();

public:
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;
	void SetShowDebug(bool InShowDebug) { bShowDebug = InShowDebug; }

protected:
    UFUNCTION()
    void OnProjectileImpact(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
    UPROPERTY(VisibleAnywhere)
    class UProjectileMovementComponent* ProjectileMovement;

    UPROPERTY(VisibleAnywhere)
    class USphereComponent* CollisionComponent;

private:
	bool bShowDebug;
};

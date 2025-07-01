// Fill out your copyright notice in the Description page of Project Settings.


#include "FKTA_Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AFKTA_Projectile::AFKTA_Projectile()
{
	bShowDebug = false;
    PrimaryActorTick.bCanEverTick = false;

    // 충돌체 생성
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
    CollisionComponent->InitSphereRadius(5.0f);
    CollisionComponent->SetCollisionProfileName("Projectile");
    RootComponent = CollisionComponent;

    // 프로젝트 타겟 충돌 이벤트 바인딩
    CollisionComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnProjectileImpact);

    // 이동 컴포넌트 생성
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComponent;
    ProjectileMovement->InitialSpeed = 1500.f;
    ProjectileMovement->MaxSpeed = 1500.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    bDestroyOnConfirmation = true; // ConfirmTargeting 하면 자동 파괴
}

void AFKTA_Projectile::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

void AFKTA_Projectile::ConfirmTargetingAndContinue()
{
	if (SourceActor)
	{
		FGameplayAbilityTargetDataHandle DataHandle;
		TargetDataReadyDelegate.Broadcast(DataHandle);
	}
}

void AFKTA_Projectile::OnProjectileImpact(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (ShouldProduceTargetData())
    {
        FGameplayAbilityTargetDataHandle DataHandle;
        FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(Hit);
        DataHandle.Add(TargetData);

        TargetDataReadyDelegate.Broadcast(DataHandle);
        ConfirmTargetingAndContinue(); // AbilityTask에서 OnTargetDataReady로 이어짐
    }
}

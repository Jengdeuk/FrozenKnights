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

    // �浹ü ����
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
    CollisionComponent->InitSphereRadius(5.0f);
    CollisionComponent->SetCollisionProfileName("Projectile");
    RootComponent = CollisionComponent;

    // ������Ʈ Ÿ�� �浹 �̺�Ʈ ���ε�
    CollisionComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnProjectileImpact);

    // �̵� ������Ʈ ����
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = CollisionComponent;
    ProjectileMovement->InitialSpeed = 1500.f;
    ProjectileMovement->MaxSpeed = 1500.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;

    bDestroyOnConfirmation = true; // ConfirmTargeting �ϸ� �ڵ� �ı�
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
        ConfirmTargetingAndContinue(); // AbilityTask���� OnTargetDataReady�� �̾���
    }
}

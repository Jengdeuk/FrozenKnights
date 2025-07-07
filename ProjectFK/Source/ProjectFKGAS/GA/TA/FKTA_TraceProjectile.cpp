// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/TA/FKTA_TraceProjectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Physics/FKCollision.h"
#include "Character/FKCharacterBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "FX/FKFXActor_Projectile.h"
#include "Tag/FKGameplayTag.h"

AFKTA_TraceProjectile::AFKTA_TraceProjectile()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    // 충돌체 생성
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
    CollisionComponent->InitSphereRadius(50.0f);
    CollisionComponent->SetCollisionProfileName(CPROFILE_FKPROJECTILE);
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnProjectileOverlap);
    CollisionComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnProjectileHitWall);

    RootComponent = CollisionComponent;

    // 이동 컴포넌트 생성
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = RootComponent;
    ProjectileMovement->InitialSpeed = 1500.f;
    ProjectileMovement->MaxSpeed = 1500.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AFKTA_TraceProjectile::BeginPlay()
{
    Super::BeginPlay();

    if (CollisionComponent)
    {
        CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    if (ProjectileMovement)
    {
        ProjectileMovement->StopMovementImmediately();
        ProjectileMovement->Deactivate();
    }
}

void AFKTA_TraceProjectile::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (FXActor && FXActor->IsPendingKillPending() == false)
    {
        FXActor->SetActorTransform(GetActorTransform());
    }

#if ENABLE_DRAW_DEBUG

    if (bShowDebug)
    {
        DrawDebugSphere(
            GetWorld(),
            GetActorLocation(),
            CollisionComponent->GetScaledSphereRadius(),
            12,
            FColor::Red,
            false, -1, 0, 1
        );
    }

#endif
}

void AFKTA_TraceProjectile::StartTargeting(UGameplayAbility* Ability)
{
    Super::StartTargeting(Ability);

    ACharacter* Character = CastChecked<ACharacter>(SourceActor);
    const FVector Forward = Character->GetActorForwardVector();
    const FVector Start = Character->GetActorLocation() + Forward * 2.f * Character->GetCapsuleComponent()->GetScaledCapsuleRadius();

    SetActorLocation(Start + FVector(0, 0, 40));

    FXActor = GetWorld()->SpawnActorDeferred<AFKFXActor_Projectile>(
        AFKFXActor_Projectile::StaticClass(),
        GetActorTransform(),
        this,
        nullptr,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn
    );

    if (FXActor)
    {
        FXActor->FinishSpawning(GetActorTransform());
    }

    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ProjectileMovement->Velocity = Forward * ProjectileMovement->InitialSpeed;
    ProjectileMovement->Activate();

    GetWorld()->GetTimerManager().SetTimer(ConfirmTimerHandle, this, &ThisClass::OnTimerFinished, 5.0f, false);
}

void AFKTA_TraceProjectile::OnTimerFinished()
{
    ConfirmTimerHandle.Invalidate();
    ConfirmTargeting();
}

void AFKTA_TraceProjectile::ConfirmTargetingAndContinue()
{
    if (SourceActor)
    {
        TargetDataReadyDelegate.Broadcast(DataHandle);
    }

    if (FXActor)
    {
        FXActor->Destroy();
    }
}

void AFKTA_TraceProjectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bHasConfirmed || OtherActor->IsA<AFKCharacterBase>() == false || OtherActor == SourceActor || Cast<AFKCharacterBase>(OtherActor)->IsPlayerCharacter())
    {
        return;
    }

    bool bValidSweep = SweepResult.Component.IsValid() && !SweepResult.Normal.IsNearlyZero();
    if (bValidSweep)
    {
        FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(SweepResult);
        DataHandle.Add(TargetData);
    }
    else
    {
        TArray<TWeakObjectPtr<AActor>> HitActors;
        HitActors.Add(OtherActor);

        FGameplayAbilityTargetData_ActorArray* ActorsData = new FGameplayAbilityTargetData_ActorArray();
        ActorsData->SetActors(HitActors);
        DataHandle = FGameplayAbilityTargetDataHandle(ActorsData);
    }

    ConfirmTargeting();
}

void AFKTA_TraceProjectile::OnProjectileHitWall(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (bHasConfirmed)
    {
        return;
    }

    ACharacter* Character = CastChecked<ACharacter>(SourceActor);

    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor);
    if (!ASC)
    {
        return;
    }

    // 이펙트 소환
    FGameplayEffectContextHandle CueContextHandle = ASC->MakeEffectContext();
    CueContextHandle.AddHitResult(Hit);
    FGameplayCueParameters CueParam;
    CueParam.EffectContext = CueContextHandle;
    ASC->ExecuteGameplayCue(GAMEPLAYCUE_CHARACTER_ATTACKHIT_MAGE, CueParam);

    ConfirmTargeting();
}

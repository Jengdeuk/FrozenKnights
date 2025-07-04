// Fill out your copyright notice in the Description page of Project Settings.


#include "FKTA_Projectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Physics/FKCollision.h"
#include "Character/FKCharacterBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "FX/FKFXActor_Projectile.h"

AFKTA_Projectile::AFKTA_Projectile()
{
    bHasConfirmed = false;
	bShowDebug = true;
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    // �浹ü ����
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
    CollisionComponent->InitSphereRadius(50.0f);
    CollisionComponent->SetCollisionProfileName(CPROFILE_FKPROJECTILE);
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnProjectileOverlap);
    CollisionComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnProjectileHitWall);

    RootComponent = CollisionComponent;

    // �̵� ������Ʈ ����
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
    ProjectileMovement->UpdatedComponent = RootComponent;
    ProjectileMovement->InitialSpeed = 1200.f;
    ProjectileMovement->MaxSpeed = 1200.f;
    ProjectileMovement->bRotationFollowsVelocity = true;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AFKTA_Projectile::BeginPlay()
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

void AFKTA_Projectile::Tick(float DeltaSeconds)
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

void AFKTA_Projectile::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();

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

void AFKTA_Projectile::OnTimerFinished()
{
    ConfirmTimerHandle.Invalidate();
    ConfirmTargeting();
}

void AFKTA_Projectile::ConfirmTargeting()
{
    Super::ConfirmTargeting();

    bHasConfirmed = true;
}

void AFKTA_Projectile::ConfirmTargetingAndContinue()
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

void AFKTA_Projectile::OnProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (bHasConfirmed || OtherActor->IsA<AFKCharacterBase>() == false || OtherActor == SourceActor || Cast<AFKCharacterBase>(OtherActor)->IsPlayerCharacter())
    {
        return;
    }

    // SweepResult�� �������� ��츦 ���
    bool bValidSweep = SweepResult.Component.IsValid() && !SweepResult.Normal.IsNearlyZero();
    if (bValidSweep)
    {
        FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(SweepResult);
        DataHandle.Add(TargetData);

        //UE_LOG(LogTemp, Warning, TEXT("[TA] Created TargetData with SweepResult: %s"), *SweepResult.ImpactPoint.ToString());
    }
    else
    {
        TArray<TWeakObjectPtr<AActor>> HitActors;
        HitActors.Add(OtherActor);

        FGameplayAbilityTargetData_ActorArray* ActorsData = new FGameplayAbilityTargetData_ActorArray();
        ActorsData->SetActors(HitActors);
        DataHandle = FGameplayAbilityTargetDataHandle(ActorsData);

        //UE_LOG(LogTemp, Warning, TEXT("[TA] Created Fallback TargetData at %s"), *GetActorLocation().ToString());
    }

    ConfirmTargeting();
}

void AFKTA_Projectile::OnProjectileHitWall(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (bHasConfirmed)
    {
        return;
    }

    ConfirmTargeting();
}
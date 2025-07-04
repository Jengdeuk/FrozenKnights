// Fill out your copyright notice in the Description page of Project Settings.


#include "FX/FKFXActor_Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "GameFramework/MovementComponent.h"

AFKFXActor_Projectile::AFKFXActor_Projectile()
{
    bReplicates = true;
    SetReplicatingMovement(true);

	PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/ParagonGideon/FX/Particles/Gideon/Abilities/Primary/FX/P_Gideon_Primary_Projectile.P_Gideon_Primary_Projectile'"));
    if (EffectRef.Object)
    {
        ParticleSystem = EffectRef.Object;
    }
}

void AFKFXActor_Projectile::BeginPlay()
{
	Super::BeginPlay();

    UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAttached(
        ParticleSystem,
        RootComponent,
        NAME_None,
        FVector::ZeroVector,
        FRotator::ZeroRotator,
        EAttachLocation::SnapToTargetIncludingScale,
        true // AutoDestroy
    );
}

void AFKFXActor_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


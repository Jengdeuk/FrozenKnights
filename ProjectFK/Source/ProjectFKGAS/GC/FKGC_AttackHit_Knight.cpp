// Fill out your copyright notice in the Description page of Project Settings.


#include "FKGC_AttackHit_Knight.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

UFKGC_AttackHit_Knight::UFKGC_AttackHit_Knight()
{
	static ConstructorHelpers::FObjectFinder<UParticleSystem> EffectRef(TEXT("/Script/Engine.ParticleSystem'/Game/ParagonGreystone/FX/Particles/Greystone/Skins/Novaborn/P_Greystone_Novaborn_Primary_Impact.P_Greystone_Novaborn_Primary_Impact'"));
	if (EffectRef.Object)
	{
		ParticleSystem = EffectRef.Object;
	}
}

bool UFKGC_AttackHit_Knight::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	const FHitResult* HitResult = Parameters.EffectContext.GetHitResult();
	if (HitResult)
	{
		UGameplayStatics::SpawnEmitterAtLocation(Target, ParticleSystem, HitResult->ImpactPoint, FRotator::ZeroRotator, true);
	}

	return false;
}

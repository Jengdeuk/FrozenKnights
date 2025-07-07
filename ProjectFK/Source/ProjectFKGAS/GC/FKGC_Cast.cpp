// Fill out your copyright notice in the Description page of Project Settings.


#include "GC/FKGC_Cast.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

UFKGC_Cast::UFKGC_Cast()
{
}

bool UFKGC_Cast::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) const
{
	UGameplayStatics::SpawnEmitterAtLocation(Target, ParticleSystem, Parameters.Location, FRotator::ZeroRotator, true);

	return false;
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FKMonsterPoolManager.generated.h"

USTRUCT()
struct FSpawnSlot
{
	GENERATED_BODY()

	UPROPERTY()
	FVector SpawnLocation;

	UPROPERTY()
	FRotator SpawnRotation;

	UPROPERTY()
	FTimerHandle SpawnTimerHandle;
};

UCLASS()
class PROJECTFK_API AFKMonsterPoolManager : public AActor
{
	GENERATED_BODY()

public:
	AFKMonsterPoolManager();

public:
	UFUNCTION()
	void InitPool(UClass* InMonsterClass, int32 Size);

	UFUNCTION()
	void DeferredSpawn(uint32 MonsterId);

	UFUNCTION()
	void Spawn(uint32 MonsterId);

private:
	UPROPERTY()
	TSubclassOf<class AFKCharacterNonPlayer> MonsterClass;

	UPROPERTY()
	TArray<FSpawnSlot> SpawnSlots;

	UPROPERTY(EditAnywhere, Category = "Pool")
	TArray<TObjectPtr<class AFKCharacterNonPlayer>> MonsterPool;
};
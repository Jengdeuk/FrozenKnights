// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FKMonsterPoolManager.generated.h"

UCLASS()
class PROJECTFK_API AFKMonsterPoolManager : public AActor
{
	GENERATED_BODY()

public:
	AFKMonsterPoolManager();

public:
	template <typename T>
	void InitPool(const int32 Size)
	{
		static_assert(TIsDerivedFrom<T, class AFKCharacterNonPlayer>::IsDerived, "T must derive from AFKCharacterNonPlayer");

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		for (int32 i = 0; i < Size; ++i)
		{
			if (AFKCharacterNonPlayer* NewMonster = GetWorld()->SpawnActor<T>(FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams))
			{
				NewMonster->Deactivate();
				MonsterPool.Add(NewMonster);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("SpawnActor failed in MonsterPool"));
			}
		}
	}

private:
	UPROPERTY(EditAnywhere, Category = "Pool")
	TArray<TObjectPtr<class AFKCharacterNonPlayer>> MonsterPool;
};
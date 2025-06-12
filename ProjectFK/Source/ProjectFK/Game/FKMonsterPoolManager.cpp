#include "Game/FKMonsterPoolManager.h"
#include "Character/FKCharacterNonPlayer.h"

AFKMonsterPoolManager::AFKMonsterPoolManager()
{
	SpawnSlots.Add({ FVector(1100, 3080, -530), FTimerHandle() });

	SpawnSlots.Add({ FVector(1940, 4290, -750), FTimerHandle() });
	SpawnSlots.Add({ FVector(2600, 4030, -750), FTimerHandle() });

	SpawnSlots.Add({ FVector(-340, 1640, -530), FTimerHandle() });
	SpawnSlots.Add({ FVector(-1180, 2100, -530), FTimerHandle() });
	SpawnSlots.Add({ FVector(-1950, 1640, -530), FTimerHandle() });

	SpawnSlots.Add({ FVector(-1190, 940, -430), FTimerHandle() });

	SpawnSlots.Add({ FVector(-530, 0, -210), FTimerHandle() });
	SpawnSlots.Add({ FVector(-2310, 0, -210), FTimerHandle() });
}

void AFKMonsterPoolManager::InitPool(UClass* InMonsterClass, int32 Size)
{
	MonsterClass = InMonsterClass;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (int32 i = 0; i < Size; ++i)
	{
		if (AFKCharacterNonPlayer* NewMonster = GetWorld()->SpawnActor<AFKCharacterNonPlayer>(MonsterClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams))
		{
			NewMonster->Deactivate();
			MonsterPool.Add(NewMonster);
			DeferredSpawn(i);
		}
	}
}

void AFKMonsterPoolManager::DeferredSpawn(uint32 MonsterId)
{
	FTimerDelegate SpawnDelegate;
	SpawnDelegate.BindUFunction(this, FName("Spawn"), MonsterId);
	GetWorld()->GetTimerManager().SetTimer(SpawnSlots[MonsterId].SpawnTimerHandle, SpawnDelegate, 5.0f, false);
}

void AFKMonsterPoolManager::Spawn(uint32 MonsterId)
{
	SpawnSlots[MonsterId].SpawnTimerHandle.Invalidate();

	for (TObjectPtr<AFKCharacterNonPlayer> Monster : MonsterPool)
	{
		if (Monster->IsActive() == false)
		{
			Monster->ActivatePoolableMonster(MonsterId, this);
			Monster->SetActorLocation(SpawnSlots[MonsterId].SpawnLocation);
			return;
		}
	}

	// 풀에 사용 가능한 오브젝트가 없으면 새로 생성
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (AFKCharacterNonPlayer* NewMonster = GetWorld()->SpawnActor<AFKCharacterNonPlayer>(MonsterClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams))
	{
		NewMonster->ActivatePoolableMonster(MonsterId, this);
		NewMonster->SetActorLocation(SpawnSlots[MonsterId].SpawnLocation);
		MonsterPool.Add(NewMonster);
	}
}

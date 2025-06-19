#include "Game/FKMonsterPoolManager.h"
#include "Character/FKCharacterNonPlayer.h"

AFKMonsterPoolManager::AFKMonsterPoolManager()
{
	SpawnSlots.Add({ FVector(-1190, 2050, -404), FRotator(0, 90, 0), FTimerHandle()});
	SpawnSlots.Add({ FVector(-1920, 1700, -404), FRotator(0, 90, 0), FTimerHandle() });
	SpawnSlots.Add({ FVector(-420, 1710, -404), FRotator(0, 90, 0), FTimerHandle() });

	SpawnSlots.Add({ FVector(-1200, 1020, -305), FRotator(0, 90, 0), FTimerHandle() });
	SpawnSlots.Add({ FVector(-2140, 10, -116), FRotator(0, 180, 0), FTimerHandle() });
	SpawnSlots.Add({ FVector(-530, 20, -74), FRotator(0, 0, 0), FTimerHandle() });

	SpawnSlots.Add({ FVector(830, 3140, -427), FRotator(0, 180, 0), FTimerHandle() });
	SpawnSlots.Add({ FVector(820, 4830, -437), FRotator(0, 180, 0), FTimerHandle() });

	SpawnSlots.Add({ FVector(2100, 4320, -647), FRotator(0, 180, 0), FTimerHandle() });
	SpawnSlots.Add({ FVector(2560, 4040, -647), FRotator(0, 0, 0), FTimerHandle() });
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
	GetWorld()->GetTimerManager().SetTimer(SpawnSlots[MonsterId].SpawnTimerHandle, SpawnDelegate, 10.0f, false);
}

void AFKMonsterPoolManager::Spawn(uint32 MonsterId)
{
	SpawnSlots[MonsterId].SpawnTimerHandle.Invalidate();

	for (TObjectPtr<AFKCharacterNonPlayer> Monster : MonsterPool)
	{
		if (Monster->IsActive() == false && Monster->IsPreparingActivate() == false)
		{
			Monster->ActivatePoolableMonster(MonsterId, this);
			Monster->SetActorLocation(SpawnSlots[MonsterId].SpawnLocation);
			Monster->SetActorRotation(SpawnSlots[MonsterId].SpawnRotation);
			Monster->SetHomePos(SpawnSlots[MonsterId].SpawnLocation);
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
		NewMonster->SetActorRotation(SpawnSlots[MonsterId].SpawnRotation);
		NewMonster->SetHomePos(SpawnSlots[MonsterId].SpawnLocation);
		MonsterPool.Add(NewMonster);
	}
}

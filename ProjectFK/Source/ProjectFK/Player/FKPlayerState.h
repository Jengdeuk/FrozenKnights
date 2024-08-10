// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FKPlayerState.generated.h"

UENUM()
enum class EPlayerClass : uint8
{
	Knight,
	Mage,
	None
};

UCLASS()
class PROJECTFK_API AFKPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AFKPlayerState();

public:
	FORCEINLINE const EPlayerClass& GetPlayerClass() const { return PlayerClass; }
	FORCEINLINE void SetPlayerClass(const EPlayerClass& InPlayerClass) { PlayerClass = InPlayerClass; }

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
	void OnRep_UpdatePlayerClass();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_UpdatePlayerClass)
	EPlayerClass PlayerClass;
};

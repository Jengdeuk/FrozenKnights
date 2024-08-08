// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FKPlayerState.generated.h"

UENUM()
enum class EPlayerClass : uint8
{
	Knight,
	Mage
};

UCLASS()
class PROJECTFK_API AFKPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AFKPlayerState();

public:
	FORCEINLINE const EPlayerClass& GetClass() const { return Class; }

protected:
	EPlayerClass Class;
};

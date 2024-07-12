// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FKCharacterBase.generated.h"

UENUM()
enum class ECharacterControlType : uint8
{
	Shoulder,
	Quater
};

UCLASS()
class PROJECTFK_API AFKCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AFKCharacterBase();

protected:
	virtual void SetCharacterControlData(const class UFKCharacterControlData* CharacterControlData);

	UPROPERTY(EditAnywhere, Category = "CharacterControl", Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UFKCharacterControlData*> CharacterControlManager;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Helm;

	UPROPERTY(EditAnywhere, Category = "Helm")
	TObjectPtr<class UStaticMesh> HelmMesh;
};
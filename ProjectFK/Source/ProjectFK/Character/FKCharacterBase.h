// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/StreamableManager.h"
#include "FKCharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FInitMeshCompletedSignature);

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

// Character Mesh Section
public:
	void MeshLoadCompleted();
	void AnimLoadCompleted();
	
protected:
	FInitMeshCompletedSignature OnMeshLoadCompleted;
	TSharedPtr<FStreamableHandle> MeshHandle;
	TSharedPtr<FStreamableHandle> AnimHandle;
};
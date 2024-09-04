// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/StreamableManager.h"
#include "FKCharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FMeshLoadCompletedSignature);
DECLARE_MULTICAST_DELEGATE(FDeadSignature);

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

public:
	FORCEINLINE virtual class UAnimMontage* GetAttackMontage() const { return AttackMontage; }
	FORCEINLINE class UFKComboActionData* GetComboActionData() const { return ComboActionData; }

protected:
	virtual void SetCharacterControlData(const class UFKCharacterControlData* CharacterControlData);

	UPROPERTY(EditAnywhere, Category = "CharacterControl", Meta = (AllowPrivateAccess = "true"))
	TMap<ECharacterControlType, class UFKCharacterControlData*> CharacterControlManager;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	TObjectPtr<class UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UFKComboActionData> ComboActionData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Stat, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UAnimMontage> DeadMontage;

// Character Mesh Section
public:
	void MeshLoadCompleted();
	void AnimLoadCompleted();
	void AttackMontageLoadCompleted();
	void DeadMontageLoadCompleted();
	void ComboActionDataLoadCompleted();
	
protected:
	FMeshLoadCompletedSignature OnMeshLoadCompleted;
	TSharedPtr<FStreamableHandle> MeshHandle;
	TSharedPtr<FStreamableHandle> AnimHandle;
	TSharedPtr<FStreamableHandle> AttackHandle;
	TSharedPtr<FStreamableHandle> DeadHandle;
	TSharedPtr<FStreamableHandle> ComboActionHandle;

// Dead Section
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetDead();
	void PlayDeadAnimation();

	FDeadSignature OnDead;

	UFUNCTION()
	void OnRep_Dead();

	UPROPERTY(ReplicatedUsing = "OnRep_Dead")
	bool bDead;
};
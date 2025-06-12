// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/StreamableManager.h"
#include "FKCharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FActiveChangedSignature);
DECLARE_MULTICAST_DELEGATE(FResourcesBindCompletedSignature);
DECLARE_MULTICAST_DELEGATE(FDeadSignature);

UENUM()
enum class ECharacterControlType : uint8
{
	Shoulder,
	Quater
};

UENUM()
enum class EResourceType : uint8
{
	Mesh,
	AnimInstance,
	AttackMontage,
	DeadMontage,
	ComboActionData,
	Count
};

UCLASS()
class PROJECTFK_API AFKCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AFKCharacterBase();

public:
	virtual void Activate();
	virtual void Deactivate();

	bool IsActive() const { return bActive; }

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

// Resource Section
public:
	virtual void OnBindResourcesCompleted();
	void CheckResourcesBindCompleted();
	void MeshLoadCompleted();
	void AnimLoadCompleted();
	void AttackMontageLoadCompleted();
	void DeadMontageLoadCompleted();
	void ComboActionDataLoadCompleted();

protected:
	FResourcesBindCompletedSignature OnResourcesBindCompleted;
	TMap<EResourceType, TSharedPtr<FStreamableHandle>> ResourceHandles;
	TMap<EResourceType, bool> bResourceBinds;

// Dead Section
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void SetDead();
	void PlayDeadAnimation();

	FDeadSignature OnDead;

	UFUNCTION()
	void OnRep_Dead();

	UPROPERTY(ReplicatedUsing = "OnRep_Dead")
	uint8 bDead : 1;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UWidgetComponent> HpBar;

public:
	FORCEINLINE bool IsPlayerCharacter() const { return bPlayerCharacter; }

protected:
	uint8 bPlayerCharacter : 1;
	FTimerHandle DeactiveTimerHandle;

// Active Section
private:
	void ChangeActive();

	FActiveChangedSignature OnActiveChanged;

	UFUNCTION()
	void OnRep_ActiveChanged();

	UPROPERTY(ReplicatedUsing = "OnRep_ActiveChanged")
	uint8 bActive : 1;
};
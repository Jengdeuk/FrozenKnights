// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FKCharacterBase.h"
#include "InputActionValue.h"
#include "FKCharacterPlayer.generated.h"

UCLASS(config=ProjectFK)
class PROJECTFK_API AFKCharacterPlayer : public AFKCharacterBase
{
	GENERATED_BODY()
	
public:
	AFKCharacterPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

// Character Control Section
protected:
	void SetCharacterControl(ECharacterControlType NewCharacterControlType);
	virtual void SetCharacterControlData(const class UFKCharacterControlData* CharacterControlData) override;

// Camera Section
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCameraComponent> FollowCamera;

// Input Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderMoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> ShoulderLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UInputAction> AttackAction;

	void ShoulderMove(const FInputActionValue& Value);
	void ShoulderLook(const FInputActionValue& Value);

	ECharacterControlType CurrentCharacterControlType;

// Character Mesh Section
public:
	void UpdateMeshFromPlayerState();
	void OnInitMeshCompleted();
	void EquipHelm();
	virtual void OnRep_PlayerState() override;

protected:
	UPROPERTY(config)
	TArray<FSoftObjectPath> PlayerMeshes;

	UPROPERTY(config)
	TArray<FSoftObjectPath> PlayerAnimInstances;

	UPROPERTY(config)
	TArray<FSoftObjectPath> PlayerAttackMontages;

	UPROPERTY(config)
	TArray<FSoftObjectPath> PlayerComboActionData;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Helm;

	UPROPERTY(EditAnywhere, Category = "Helm")
	TObjectPtr<class UStaticMesh> HelmMesh;
};

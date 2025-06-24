// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/FKCharacterBase.h"
#include "InputActionValue.h"
#include "FKCharacterPlayer.generated.h"

UCLASS(config = ProjectFK)
class PROJECTFK_API AFKCharacterPlayer : public AFKCharacterBase
{
	GENERATED_BODY()
	
public:
	AFKCharacterPlayer();

public:
	virtual void Activate() override;
	virtual void Deactivate() override;

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void SetDead() override;

public:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCPlayAttackMontage();

	UFUNCTION(Client, Unreliable)
	void ClientRPCPlayAttackMontage(AFKCharacterPlayer* CharacterToPlay);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCMontageJumpToSection(FName SectionName);

	UFUNCTION(Client, Unreliable)
	void ClientRPCMontageJumpToSection(AFKCharacterPlayer* CharacterToPlay, FName SectionName);

	void PlayAttackMontage();

	void JumpMontageToSection(FName SectionName);

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

// Resource Section
protected:
	UPROPERTY(config)
	TArray<FSoftObjectPath> Meshes;

	UPROPERTY(config)
	TArray<FSoftObjectPath> AnimInstances;

	UPROPERTY(config)
	TArray<FSoftObjectPath> StartMontages;

	UPROPERTY(config)
	TArray<FSoftObjectPath> AttackMontages;

	UPROPERTY(config)
	TArray<FSoftObjectPath> DeadMontages;

	UPROPERTY(config)
	TArray<FSoftObjectPath> ComboActionDatas;

// Character Mesh Section
public:
	void BindCharacterResources();
	virtual void OnBindResourcesCompleted() override;
	void EquipHelm();
	virtual void OnRep_PlayerState() override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Helm;

	UPROPERTY(EditAnywhere, Category = "Helm")
	TObjectPtr<class UStaticMesh> HelmMesh;

// Collider Section
private:
	UFUNCTION()
	void OnCharacterOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCharacterOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USphereComponent> CharacterOverlapDetector;
};

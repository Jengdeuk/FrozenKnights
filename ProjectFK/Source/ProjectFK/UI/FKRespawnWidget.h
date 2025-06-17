// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FKRespawnWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFK_API UFKRespawnWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	void SetOwnerCharacter(class AFKCharacterPlayer* InCharacter);
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY()
	TObjectPtr<class AFKCharacterPlayer> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<class UTextBlock> RemainingTimeText;
};

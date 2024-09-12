// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AnimNotify_GASFalling.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFKGAS_API UAnimNotify_GASFalling : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UAnimNotify_GASFalling();

protected:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Meta = (Category = "Event"))
	FGameplayTag TriggerGameplayTag;
};

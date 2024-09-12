// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify_GASFalling.h"
#include "AbilitySystemBlueprintLibrary.h"

UAnimNotify_GASFalling::UAnimNotify_GASFalling()
{
}

FString UAnimNotify_GASFalling::GetNotifyName_Implementation() const
{
	return TEXT("GASFalling");
}

void UAnimNotify_GASFalling::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerActor, TriggerGameplayTag, FGameplayEventData());
		}
	}
}

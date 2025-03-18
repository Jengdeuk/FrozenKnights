// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FKAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FKAI.h"

AFKAIController::AFKAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBAssetRef(TEXT("/Script/AIModule.BlackboardData'/Game/FrozenKnights/AI/BB_FKCharacter.BB_FKCharacter'"));
	if (nullptr != BBAssetRef.Object)
	{
		BBAsset = BBAssetRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTAssetRef(TEXT("/Script/AIModule.BehaviorTree'/Game/FrozenKnights/AI/BT_FKCharacter.BT_FKCharacter'"));
	if (nullptr != BTAssetRef.Object)
	{
		BTAsset = BTAssetRef.Object;
	}
}

void AFKAIController::RunAI()
{
	UBlackboardComponent* BlackboardPtr = Blackboard.Get();
	if (UseBlackboard(BBAsset, BlackboardPtr))
	{
		Blackboard->SetValueAsVector(BBKEY_HOMEPOS, GetPawn()->GetActorLocation());

		bool RunResult = RunBehaviorTree(BTAsset);
		ensure(RunResult);
	}
}

void AFKAIController::StopAI()
{
	UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
	if (BTComponent)
	{
		BTComponent->StopTree();
	}
}

void AFKAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunAI();
}

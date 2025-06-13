// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/FKAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "FKAI.h"

AFKAIController::AFKAIController()
{
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

void AFKAIController::SetHomePos(FVector InPosition)
{
	Blackboard->SetValueAsVector(BBKEY_HOMEPOS, InPosition);
}

void AFKAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunAI();
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "FKMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTFK_API AFKMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AFKMenuPlayerController();

public:
	virtual void BeginPlay() override;

public:
	void CreateServer();
	void JoinServer();

private:
	void OnCreateSessionComplete(FName SessionName, bool bSuccess);
	void OnFindSessionsComplete(bool bSuccess);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

private:
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UFKMainMenuWidget> MenuUIClass;

	UPROPERTY()
	TObjectPtr<class UFKMainMenuWidget> MenuUI;
};

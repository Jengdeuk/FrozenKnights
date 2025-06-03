#include "Player/FKMenuPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/FKMainMenuWidget.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

// ȭ�� �α�� ��ũ��
#define	LOG_SCREEN(Format, ...) \
	if (GEngine)\
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))

AFKMenuPlayerController::AFKMenuPlayerController()
{
	static ConstructorHelpers::FClassFinder<UFKMainMenuWidget> MenuUIClassRef(TEXT("/Game/FrozenKnights/UI/WBP_MainMenu.WBP_MainMenu_C"));
	if (MenuUIClassRef.Class)
	{
		MenuUIClass = MenuUIClassRef.Class;
	}
}

void AFKMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	MenuUI = CreateWidget<UFKMainMenuWidget>(this, MenuUIClass);
	if (MenuUI)
	{
		MenuUI->AddToViewport();
	}

	SetShowMouseCursor(true);
	SetInputMode(FInputModeUIOnly());
	
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	if (OSS)
	{
		OSS->GetSessionInterface();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Found Subsystem %s"), *OSS->GetSubsystemName().ToString())
			);
		}
	}
}

void AFKMenuPlayerController::CreateServer()
{
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	if (!OSS) return;

	IOnlineSessionPtr Session = OSS->GetSessionInterface();
	if (!Session.IsValid()) return;

	// NAME_GameSession �̸��� ������ �����ϴ��� �˻��Ͽ� �ı�
	if (FNamedOnlineSession* ExistingSession = Session->GetNamedSession(NAME_GameSession))
	{
		Session->DestroySession(NAME_GameSession);
		LOG_SCREEN("Destroy session: %s", NAME_GameSession);
	}

	// ��������Ʈ ����
	//Session->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionComplete);
	Session->OnCreateSessionCompleteDelegates.AddUObject(this, &AFKMenuPlayerController::OnCreateSessionComplete);

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->NumPublicConnections = 4;		// ���Ǵ� �÷��̾� ��
	SessionSettings->bShouldAdvertise = true;		// ����Ǵ� �������� ���� ��������
	SessionSettings->bAllowJoinInProgress = true;	// ���� �����߿� ���� ���
	SessionSettings->bIsLANMatch = false;			// LAN������ ����ǰ� �ܺ� �������� ����
	SessionSettings->bIsDedicated = false;			// ��������Ƽ�� �������� (���� ������ �ƴ���)
	SessionSettings->bUsesPresence = true;			// Presence ��� (���� ������ ���� ������ ǥ���ϴµ�)
	SessionSettings->bAllowJoinViaPresence = true;	// Presence�� ���� ���� ���
	SessionSettings->bUseLobbiesIfAvailable = true; // �÷����� �����ϴ� ��� �κ� API ���

	// FOnlineSessionSettings() �ڵ� ����
	// ������ MatchType�� ��ο��� ����, �¶��� ���񽺿� �� �����͸� ���� ���� ȫ�� �ɼ����� ����
	SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// ���� ����
	if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
		Session->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
}

void AFKMenuPlayerController::JoinServer()
{
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	if (!OSS) return;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 10;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	IOnlineSessionPtr Session = OSS->GetSessionInterface();
	if (Session.IsValid())
	{
		// ��������Ʈ ����
		//OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionComplete);
		Session->OnFindSessionsCompleteDelegates.AddUObject(this, &AFKMenuPlayerController::OnFindSessionsComplete);

		// Find Game Session
		SessionSearch = MakeShareable(new FOnlineSessionSearch());
		SessionSearch->MaxSearchResults = 100;	// �˻� ����� ������ ���� �� �ִ밪
		SessionSearch->bIsLanQuery = false;		// LAN ��� ����
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); // ã�� ���� ������ Presence�� ����

		// ���� �˻�
		if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
			Session->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
	}
}

void AFKMenuPlayerController::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	if (!bSuccess)
	{
		LOG_SCREEN("Failed CreateSession()");
		return;
	}

	LOG_SCREEN("Successful CreateSession()");
	UGameplayStatics::OpenLevel(GetWorld(), FName("MainLevel"), true, "listen");
}

void AFKMenuPlayerController::OnFindSessionsComplete(bool bSuccess)
{
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	IOnlineSessionPtr Session = OSS->GetSessionInterface();
	if (Session.IsValid() == false)
	{
		LOG_SCREEN("Session Interface is Invalid");
		return;
	}

	if (!bSuccess)
	{
		LOG_SCREEN("Failed FindSession()");
		return;
	}

	LOG_SCREEN("======== Search Result ========");

	for (auto Result : SessionSearch->SearchResults)
	{
		FString Id = Result.GetSessionIdStr();
		FString User = Result.Session.OwningUserName;

		// ��ġ Ÿ�� Ȯ���ϱ�
		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

		// ã�� ������ ���� ����ϱ�
		LOG_SCREEN("Session ID: %s / Owner: %s", *Id, *User);

		// ������ ��ġ Ÿ���� "FreeForAll"�� ��� ���� ����
		if (MatchType == FString("FreeForAll"))
		{
			LOG_SCREEN("Joining Match Type: %s", *MatchType);

			// ��������Ʈ ����
			//OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionComplete);
			Session->OnJoinSessionCompleteDelegates.AddUObject(this, &AFKMenuPlayerController::OnJoinSessionComplete);

			// ���� ����
			if (const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
				Session->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
		}
	}
}

void AFKMenuPlayerController::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OSS = IOnlineSubsystem::Get();
	IOnlineSessionPtr Session = OSS->GetSessionInterface();

	if (Session.IsValid() == false)
	{
		LOG_SCREEN("Session Interface is Invalid");
		return;
	}

	if (Result != EOnJoinSessionCompleteResult::Type::Success)
	{
		LOG_SCREEN("Failed JoinSession() - %d", Result);
		return;
	}

	// ���ǿ� ���������� �����ϸ� IP Address ���ͼ� �ش� ������ ����
	FString Address;
	if (Session->GetResolvedConnectString(NAME_GameSession, Address))
	{
		LOG_SCREEN("IP Address: %s", *Address);

		if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}
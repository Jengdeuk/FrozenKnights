#include "Player/FKMenuPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/FKMainMenuWidget.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

// 화면 로깅용 매크로
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

	// NAME_GameSession 이름의 세션이 존재하는지 검사하여 파괴
	if (FNamedOnlineSession* ExistingSession = Session->GetNamedSession(NAME_GameSession))
	{
		Session->DestroySession(NAME_GameSession);
		LOG_SCREEN("Destroy session: %s", NAME_GameSession);
	}

	// 델리게이트 연결
	//Session->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionComplete);
	Session->OnCreateSessionCompleteDelegates.AddUObject(this, &AFKMenuPlayerController::OnCreateSessionComplete);

	TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	SessionSettings->NumPublicConnections = 4;		// 허용되는 플레이어 수
	SessionSettings->bShouldAdvertise = true;		// 광고되는 세션인지 개인 세션인지
	SessionSettings->bAllowJoinInProgress = true;	// 세션 진행중에 참여 허용
	SessionSettings->bIsLANMatch = false;			// LAN에서만 실행되고 외부 공개되지 않음
	SessionSettings->bIsDedicated = false;			// 데디케이티드 서버인지 (리슨 서버가 아닌지)
	SessionSettings->bUsesPresence = true;			// Presence 사용 (유저 정보에 세션 정보를 표시하는듯)
	SessionSettings->bAllowJoinViaPresence = true;	// Presence를 통해 참여 허용
	SessionSettings->bUseLobbiesIfAvailable = true; // 플랫폼이 지원하는 경우 로비 API 사용

	// FOnlineSessionSettings() 코드 참고
	// 세션의 MatchType을 모두에게 열림, 온라인 서비스와 핑 데이터를 통해 세션 홍보 옵션으로 설정
	SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// 세션 생성
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
		// 델리게이트 연결
		//OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionComplete);
		Session->OnFindSessionsCompleteDelegates.AddUObject(this, &AFKMenuPlayerController::OnFindSessionsComplete);

		// Find Game Session
		SessionSearch = MakeShareable(new FOnlineSessionSearch());
		SessionSearch->MaxSearchResults = 100;	// 검색 결과로 나오는 세션 수 최대값
		SessionSearch->bIsLanQuery = false;		// LAN 사용 여부
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); // 찾을 세션 쿼리를 Presence로 설정

		// 세션 검색
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

		// 매치 타입 확인하기
		FString MatchType;
		Result.Session.SessionSettings.Get(FName("MatchType"), MatchType);

		// 찾은 세션의 정보 출력하기
		LOG_SCREEN("Session ID: %s / Owner: %s", *Id, *User);

		// 세션의 매치 타입이 "FreeForAll"일 경우 세션 참가
		if (MatchType == FString("FreeForAll"))
		{
			LOG_SCREEN("Joining Match Type: %s", *MatchType);

			// 델리게이트 연결
			//OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionComplete);
			Session->OnJoinSessionCompleteDelegates.AddUObject(this, &AFKMenuPlayerController::OnJoinSessionComplete);

			// 세션 참가
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

	// 세션에 정상적으로 참가하면 IP Address 얻어와서 해당 서버에 접속
	FString Address;
	if (Session->GetResolvedConnectString(NAME_GameSession, Address))
	{
		LOG_SCREEN("IP Address: %s", *Address);

		if (APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}
}
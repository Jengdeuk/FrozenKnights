// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FKRespawnWidget.h"
#include "Character/FKCharacterPlayer.h"
#include "Components/TextBlock.h"

void UFKRespawnWidget::NativeConstruct()
{
    Super::NativeConstruct();

    RemainingTimeText = Cast<UTextBlock>(GetWidgetFromName(TEXT("TxtRemainingTime")));
    ensure(RemainingTimeText);
}

void UFKRespawnWidget::SetOwnerCharacter(AFKCharacterPlayer* InCharacter)
{
	OwnerCharacter = InCharacter;
}

void UFKRespawnWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (OwnerCharacter)
    {
        int32 RemainingSeconds = FMath::Clamp(FMath::RoundToInt(OwnerCharacter->GetRespawnRemainingTime()), 0, 10);
        if (RemainingSeconds == 0)
        {
            SetVisibility(ESlateVisibility::Hidden);
            return;
        }

        RemainingTimeText->SetText(FText::FromString(FString::Printf(TEXT("%d"), RemainingSeconds)));
    }
}
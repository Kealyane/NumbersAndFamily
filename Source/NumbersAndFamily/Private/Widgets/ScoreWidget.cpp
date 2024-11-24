// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ScoreWidget.h"

#include "Components/TextBlock.h"

void UScoreWidget::UpdateScoreText(uint8 ScoreValue)
{
	FString ScoreString = FString::Printf(TEXT("%d"), ScoreValue);
	Score->SetText(FText::FromString(ScoreString));
	OnScoreUpdated.Broadcast();
}

void UScoreWidget::SetScoreParams(bool IsPlayer1, uint8 LineParam, bool bIsTotalScoreParam = false)
{
	if (IsPlayer1)
	{
		OwningPlayer = EPosition::LEFT;
	}
	else
	{
		OwningPlayer = EPosition::RIGHT;
	}
	SetScoreCoordParams(LineParam, bIsTotalScoreParam);
}

void UScoreWidget::SetScoreCoordParams(uint8 LineParam, bool bIsTotalScoreParam)
{
	Line = LineParam;
	bIsTotalScore = bIsTotalScoreParam;
	InitScore();
}

void UScoreWidget::InitScore()
{
	FString ScoreString = FString::Printf(TEXT("%d"), 0);
	Score->SetText(FText::FromString(ScoreString));
}

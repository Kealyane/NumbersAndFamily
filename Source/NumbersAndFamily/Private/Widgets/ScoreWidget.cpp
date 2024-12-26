// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ScoreWidget.h"

#include "Components/TextBlock.h"

void UScoreWidget::UpdateScoreText(int32 ScoreValue)
{
	FString ScoreString = FString::Printf(TEXT("%d"), ScoreValue);
	Score->SetText(FText::FromString(ScoreString));
	OnScoreUpdated.Broadcast();
}

void UScoreWidget::InitScore()
{
	FString ScoreString = FString::Printf(TEXT("%d"), 0);
	Score->SetText(FText::FromString(ScoreString));
}

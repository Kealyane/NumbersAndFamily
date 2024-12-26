// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/EndGameWidget.h"

#include "Components/TextBlock.h"

void UEndGameWidget::SetEndGameText(bool bHasWon, int32 Score)
{
	FString TextString;
	if (bHasWon)
	{
		TextString = FString::Printf(TEXT("YOU WON\nYour Score is %d"), Score);
	}
	else
	{
		TextString = FString::Printf(TEXT("YOU LOST\nYour Score is %d"), Score);
	}
	EndGameText->SetText(FText::FromString(TextString));
}

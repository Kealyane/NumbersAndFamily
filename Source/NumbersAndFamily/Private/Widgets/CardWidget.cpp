// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CardWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UCardWidget::SetSlotParams(bool IsPlayer1, uint8 LineParam, uint8 ColParam)
{
	if (IsPlayer1)
	{
		OwningPlayer = EPosition::LEFT;
	}
	else
	{
		OwningPlayer = EPosition::RIGHT;
	}

	Line = LineParam;
	Col = ColParam;
}

void UCardWidget::CardClicked()
{
	if (bCanBeClicked)
	{
		OnClick.Broadcast(OwningPlayer, Line, Col);
	}
}

void UCardWidget::SelectCard(bool bIsSelected)
{
	bIsCardSlotSelected = bIsSelected;
	
	if (bIsSelected)
	{
		ButtonSlot->SetBackgroundColor(FLinearColor::Green);
		OnCardSelected.Broadcast();
	}
	else
	{
		ButtonSlot->SetBackgroundColor(FLinearColor::White);
		OnCardUnselected.Broadcast();
	}
}

void UCardWidget::ShowCard(UTexture2D* CardImage)
{
	bIsCardOccupied = true;
	ImageCard->SetBrushFromTexture(CardImage);
	ImageCard->SetOpacity(1.f);
	OnCardPlace.Broadcast();
}

void UCardWidget::HideCard()
{
	bIsCardOccupied = false;
	ImageCard->SetOpacity(0.f);
}

void UCardWidget::EnableHighlight()
{
	bCanBeClicked = true;
	ButtonSlot->SetBackgroundColor(FLinearColor::Yellow);
}

void UCardWidget::DisableHighlight()
{
	if (bIsHandSelectionOnGoing)
	{
		ButtonSlot->SetBackgroundColor(FLinearColor::Yellow);
	}
	else
	{
		bCanBeClicked = false;
		ButtonSlot->SetBackgroundColor(FLinearColor::White);
	}
}

void UCardWidget::SpecialCardFirstChoiceSelected()
{
	bCanBeClicked = true;
	ButtonSlot->SetBackgroundColor(FLinearColor::Blue);
}

void UCardWidget::SwitchTexture(UTexture2D* CardImage)
{
	ImageCard->SetBrushFromTexture(CardImage);
}



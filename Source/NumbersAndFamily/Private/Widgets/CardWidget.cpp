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

void UCardWidget::SetSlotCoordParams(uint8 LineParam, uint8 ColParam)
{
	Line = LineParam;
	Col = ColParam;
}

void UCardWidget::CardClicked()
{
	OnClick.Broadcast(OwningPlayer, Line, Col);
}

void UCardWidget::ActivateButton()
{
	ButtonSlot->SetIsEnabled(true);
}

void UCardWidget::DeactivateButton()
{
	ButtonSlot->SetIsEnabled(false);
}

void UCardWidget::ShowCard(UTexture2D* CardImage)
{
	ImageCard->SetBrushFromTexture(CardImage);
	ImageCard->SetOpacity(1.f);
	OnCardPlace.Broadcast();
}

void UCardWidget::HideCard()
{
	ImageCard->SetOpacity(0.f);
}

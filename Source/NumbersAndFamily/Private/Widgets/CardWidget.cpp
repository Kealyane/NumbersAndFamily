// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CardWidget.h"

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
		OnClickSlot.Broadcast(OwningPlayer, Line, Col);
	}
}

void UCardWidget::SelectCard(bool bIsSelected)
{
	bIsCardSlotSelected = bIsSelected;
	
	if (bIsSelected)
	{
		OnCardSelected.Broadcast(); // green
	}
	else
	{
		OnCardUnselected.Broadcast(); // white
	}
}

void UCardWidget::ShowCard(UTexture2D* InCardImage)
{
	bIsCardOccupied = true;
	OnCardPlace.Broadcast(InCardImage); // alpha  1
}

void UCardWidget::HideCard()
{
	bIsCardOccupied = false;
	OnCardHidden.Broadcast(); // alpha 0
}

void UCardWidget::EnableHighlight()
{
	bCanBeClicked = true;
	OnEnableHighlight.Broadcast(); // yellow
}

void UCardWidget::DisableHighlight()
{
	if (bIsHandSelectionOnGoing)
	{
		OnEnableHighlight.Broadcast(); // yellow
	}
	else
	{
		if (bCanBeClicked)
		{
			bCanBeClicked = false;
			OnDisableHighlight.Broadcast(); // white
		}
	}
}

void UCardWidget::SpecialCardFirstChoiceSelected()
{
	bCanBeClicked = true;
	OnSpecialCardEnableSelection.Broadcast(); // blue
}

void UCardWidget::SwitchTexture(UTexture2D* CardImage)
{
	OnSetImage.Broadcast(CardImage);
}

void UCardWidget::CardDestroyedByNumEffect()
{
	OnCardDestroyBySameNumber.Broadcast();
}

void UCardWidget::CardDestroyByFamEffect()
{
	OnCardDestroyedByFamily.Broadcast();
}

void UCardWidget::ShowCardAnim()
{
	OnShowCardAnim.Broadcast();
}



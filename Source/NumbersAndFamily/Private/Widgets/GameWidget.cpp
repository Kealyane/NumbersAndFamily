// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameWidget.h"

#include "NumbersAndFamily/NumbersAndFamily.h"
#include "Widgets/CardWidget.h"
#include "Widgets/ScoreWidget.h"
#include "Components/TextBlock.h"

void UGameWidget::ButtonCardClicked(FVector ButtonPos)
{
	OnClickCard.Broadcast(ButtonPos);
}

void UGameWidget::ShowDeckCard(EPosition PlayerPos, UTexture2D* CardRecto)
{
	OnCardDrawFromDeck.Broadcast(PlayerPos, CardRecto);
}

void UGameWidget::ShowActivePlayer(EPosition ActivePlayer)
{

	if (ActivePlayer == EPosition::LEFT)
	{
		Text_P1_Name->SetColorAndOpacity(EnableColor);
		Text_P2_Name->SetColorAndOpacity(DisableColor);
	}
	else
	{
		Text_P1_Name->SetColorAndOpacity(DisableColor);
		Text_P2_Name->SetColorAndOpacity(EnableColor);
	}
	
}

void UGameWidget::ResetPlayerCardDeck(EPosition PlayerPos)
{
	if (PlayerPos == EPosition::LEFT)
	{
		//P1Deck_img->SetBrushFromTexture(UTexture2D::CreateTransient(1, 1));
	}
}

UCardWidget* UGameWidget::GetCardWidget(EPosition PlayerPos, uint8 CardPos) const
{
	if (PlayerPos == EPosition::LEFT)
	{
		if (CardPos == 1)
		{
			return WBP_Card_P1_Pocket;
		}
		return WBP_Card_P1_Pocket_1;
	}
	if (CardPos == 1)
	{
		return WBP_Card_P2_Pocket;
	}
	return WBP_Card_P2_Pocket_1;
}

void UGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	BoardSlots.SetNum(3);
	BoardSlots[0] = { WBP_Card_00, WBP_Card_01, WBP_Card_02, WBP_Card_03, WBP_Card_04, WBP_Card_05 };
	BoardSlots[1] = { WBP_Card_10, WBP_Card_11, WBP_Card_12, WBP_Card_13, WBP_Card_14, WBP_Card_15 };
	BoardSlots[2] = { WBP_Card_20, WBP_Card_21, WBP_Card_22, WBP_Card_23, WBP_Card_24, WBP_Card_25 };

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			BoardSlots[i][j]->SetSlotParams(j < 3, i, j);
		}
	}

	TArray<TArray<TObjectPtr<UCardWidget>>> PlayerLeftBoardSlots;
	PlayerLeftBoardSlots.SetNum(3);
	PlayerLeftBoardSlots[0] = { WBP_Card_00, WBP_Card_01, WBP_Card_02 };
	PlayerLeftBoardSlots[1] = { WBP_Card_10, WBP_Card_11, WBP_Card_12 };
	PlayerLeftBoardSlots[2] = { WBP_Card_20, WBP_Card_21, WBP_Card_22 };

	TArray<TArray<TObjectPtr<UCardWidget>>> PlayerRightBoardSlots;
	PlayerRightBoardSlots.SetNum(3);
	PlayerRightBoardSlots[0] = { WBP_Card_03, WBP_Card_04, WBP_Card_05 };
	PlayerRightBoardSlots[1] = { WBP_Card_13, WBP_Card_14, WBP_Card_15 };
	PlayerRightBoardSlots[2] = { WBP_Card_23, WBP_Card_24, WBP_Card_25 };

	PlayerBoardSlots =
	{
		{EPosition::LEFT, PlayerLeftBoardSlots },
		{EPosition::RIGHT, PlayerRightBoardSlots }
	};

	WBP_Card_P1_Pocket->SetSlotParams(true, 3, 1);
	WBP_Card_P1_Pocket_1->SetSlotParams(true, 3, 2);
	WBP_Card_P2_Pocket->SetSlotParams(false, 3, 3);
	WBP_Card_P2_Pocket_1->SetSlotParams(false, 3, 4);
	
	PlayerPockets =
	{
		{EPosition::LEFT, { WBP_Card_P1_Pocket, WBP_Card_P1_Pocket_1 } },
		{EPosition::RIGHT, { WBP_Card_P2_Pocket, WBP_Card_P2_Pocket_1 } }
	};
	
	Scores =
	{
		{EPosition::LEFT, { WBP_ScoreP1_0, WBP_ScoreP1_1, WBP_ScoreP1_2, WBP_ScoreP1_Total } },
		{EPosition::RIGHT, { WBP_ScoreP2_0, WBP_ScoreP2_1, WBP_ScoreP2_2, WBP_ScoreP2_Total } }
	};

	for (auto& ScorePlayer : Scores)
	{
		for (auto& Score : ScorePlayer.Value)
		{
			Score->InitScore();
		}
	}
}
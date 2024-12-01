// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameWidget.h"

#include "NumbersAndFamily/NumbersAndFamily.h"
#include "Widgets/CardWidget.h"
#include "Widgets/ScoreWidget.h"
#include "Components/TextBlock.h"
#include "Player/NAFPlayerController.h"
#include "Player/NAFPlayerState.h"

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

void UGameWidget::StartHandSelection(EPosition ActivePlayer)
{
	auto CurrentPlayerHand = PlayerPockets[ActivePlayer];
	for (const auto& CardWidgets : CurrentPlayerHand)
	{
		CardWidgets->bIsHandSelectionOnGoing = true;
	}
}

void UGameWidget::EndHandSelection(EPosition ActivePlayer)
{
	auto CurrentPlayerHand = PlayerPockets[ActivePlayer];
	for (const auto& CardWidgets : CurrentPlayerHand)
	{
		CardWidgets->bIsHandSelectionOnGoing = false;
	}
}


void UGameWidget::ActivateHighlight(EPosition PlayerId, ECardType CardType)
{
	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Turquoise,
	FString::Printf(TEXT("GameWidget : active highlight id %s card %s"), *EnumHelper::ToString(PlayerId), *EnumCardTypeHelper::ToString(CardType)));
	
	if (CardType == ECardType::NORMAL)
	{
		for (auto Row : PlayerBoardSlots[PlayerId])
		{
			for (int i = 0; i < Row.Num(); i++)
			{
				if (!Row[i]->bIsCardOccupied)
				{
					Row[i]->EnableHighlight();
					break;
				}
			}
		}
		return;
	}
	if (CardType == ECardType::SWITCH || CardType == ECardType::STEAL)
	{
		// if Switch : PlayerId needs to be the same as selected card
		// if Steal : PlayerId needs to be opponent 
		EPosition PlayerIdToHighlight = CardType == ECardType::SWITCH ? PlayerId
						: (PlayerId == EPosition::LEFT) ? EPosition::RIGHT : EPosition::LEFT;
		
		if (FirstCardSelected == nullptr)
		{
			for (auto RowLeft : PlayerBoardSlots[EPosition::LEFT])
			{
				for (int i = 0; i < RowLeft.Num(); i++)
				{
					if (RowLeft[i]->bIsCardOccupied)
					{
						RowLeft[i]->EnableHighlight();
					}
				}
			}
			for (auto RowRight : PlayerBoardSlots[EPosition::RIGHT])
			{
				for (int i = 0; i < RowRight.Num(); i++)
				{
					if (RowRight[i]->bIsCardOccupied)
					{
						RowRight[i]->EnableHighlight();
					}
				}
			}
		}
		else
		{
			DeactivateHighlight();
			for (auto RowLeft : PlayerBoardSlots[PlayerIdToHighlight])
			{
				for (int i = 0; i < RowLeft.Num(); i++)
				{
					if (RowLeft[i]->bIsCardOccupied)
					{
						RowLeft[i]->EnableHighlight();
					}
				}
			}
		}
		return;
	}
	if (CardType == ECardType::COPY)
	{
		if (FirstCardSelected == nullptr)
		{
			for (auto RowLeft : PlayerBoardSlots[PlayerId])
			{
				for (int i = 0; i < RowLeft.Num(); i++)
				{
					if (RowLeft[i]->bIsCardOccupied)
					{
						RowLeft[i]->EnableHighlight();
					}
				}
			}
		}
		else
		{
			DeactivateHighlight();
			for (auto Row : PlayerBoardSlots[PlayerId])
			{
				for (int i = 0; i < Row.Num(); i++)
				{
					if (!Row[i]->bIsCardOccupied)
					{
						Row[i]->EnableHighlight();
						break;
					}
				}
			}
		}
	}
}

void UGameWidget::DeactivateHighlight()
{
	for (int i = 0; i < BoardSlots.Num(); i++)
	{
		for (int j = 0; j < BoardSlots[i].Num(); j++)
		{
			if (BoardSlots[i][j])
			{
				BoardSlots[i][j]->DisableHighlight();
			}
		}
	}
}

void UGameWidget::ActiveHandHighlight(EPosition PlayerId)
{
	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Turquoise,
	FString::Printf(TEXT("GameWidget : ActiveHandHighlight id %s"), *EnumHelper::ToString(PlayerId)));
	
	auto CurrentPlayerHand = PlayerPockets[PlayerId];
	for (const auto& CardWidgets : CurrentPlayerHand)
	{
		CardWidgets->EnableHighlight();	
	}
}

void UGameWidget::DeactivateHandHighlight(EPosition PlayerId)
{
	auto CurrentPlayerHand = PlayerPockets[PlayerId];
	for (const auto& CardWidgets : CurrentPlayerHand)
	{
		CardWidgets->DisableHighlight();	
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
	// PlayerLeftBoardSlots[0] = { WBP_Card_00, WBP_Card_01, WBP_Card_02 };
	// PlayerLeftBoardSlots[1] = { WBP_Card_10, WBP_Card_11, WBP_Card_12 };
	// PlayerLeftBoardSlots[2] = { WBP_Card_20, WBP_Card_21, WBP_Card_22 };
	PlayerLeftBoardSlots[0] = { WBP_Card_02, WBP_Card_01, WBP_Card_00 };
	PlayerLeftBoardSlots[1] = { WBP_Card_12, WBP_Card_11, WBP_Card_10 };
	PlayerLeftBoardSlots[2] = { WBP_Card_22, WBP_Card_21, WBP_Card_20 };

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

	BindCardWidget();
}

void UGameWidget::BindCardWidget()
{
	// Board
	for (int i = 0; i < BoardSlots.Num(); i++)
	{
		for (int j = 0; j < BoardSlots[i].Num(); j++)
		{
			if (BoardSlots[i][j])
			{
				BoardSlots[i][j]->OnClick.AddDynamic(this, &UGameWidget::OnBoardCardSelected);
			}
		}
	}

	// Pocket/Hand
	for (auto& PlayerPocket : PlayerPockets)
	{
		for (auto& PocketCard : PlayerPocket.Value) 
		{
			if (PocketCard)
			{
				PocketCard->OnClick.AddDynamic(this, &UGameWidget::OnHandCardSelected);
			}
		}
	}
}

void UGameWidget::OnHandCardSelected(EPosition Player, uint8 LineSelect, uint8 ColSelect)
{
	if (LineSelect != 3) return;
	if (Player == GetPlayerId())
	{
		uint8 pos = 0;
		switch (ColSelect)
		{
			case 1 : pos = 0; break;
			case 2 : pos = 1; break;
			case 3: pos = 0; break;
			case 4 : pos = 1; break;
			default: pos = 0;	
		}
		UCardWidget* NewCard = PlayerPockets[Player][pos];
		if (SelectedHandCard)
		{
			if (SelectedHandCard != NewCard)
			{
				SelectedHandCard->SelectCard(false);
				SelectedHandCard->DisableHighlight();
				
				SelectedHandCard = NewCard;
				SelectedHandCard->SelectCard(true);

				bIsHandChoiceDone = true;
				OnSendHandCardSelected.Broadcast(pos+1);
			}
			else
			{
				SelectedHandCard->SelectCard(false);
				SelectedHandCard->DisableHighlight();
				
				SelectedHandCard = nullptr;
				bIsHandChoiceDone = false;
				OnSendHandCardSelected.Broadcast(3);
			}
		}
		else
		{
			SelectedHandCard = NewCard;
			SelectedHandCard->SelectCard(true);
			bIsHandChoiceDone = true;
			OnSendHandCardSelected.Broadcast(pos+1);
		}
	}
}

void UGameWidget::OnBoardCardSelected(EPosition Player, uint8 LineSelect, uint8 ColSelect)
{
	// TODO
}

EPosition UGameWidget::GetPlayerId()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ANAFPlayerController* NafPC = Cast<ANAFPlayerController>(PlayerController))
		{
			return NafPC->GetPlayerState<ANAFPlayerState>()->Id;
		}
	}
	return EPosition::SERVER;
}

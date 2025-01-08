// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameWidget.h"

#include "NumbersAndFamily/NumbersAndFamily.h"
#include "Widgets/CardWidget.h"
#include "Widgets/ScoreWidget.h"
#include "Player/NAFPlayerController.h"
#include "Player/NAFPlayerState.h"


void UGameWidget::ShowActivePlayer(EPosition ActivePlayer)
{

	if (ActivePlayer == EPosition::LEFT)
	{
		Player1IsActive.Broadcast();
		Player2IsInactive.Broadcast();
	}
	else
	{
		Player1IsInactive.Broadcast();
		Player2IsActive.Broadcast();
	}
	
}

void UGameWidget::StartHandSelection(EPosition ActivePlayer)
{
	auto CurrentPlayerHand = PlayerPockets[ActivePlayer];
	for (const auto& CardWidgets : CurrentPlayerHand)
	{
		CardWidgets->bIsHandSelectionOnGoing = true;
		CardWidgets->OnDisableHighlight.Broadcast();
	}
}

void UGameWidget::EndHandSelection(EPosition ActivePlayer)
{
	auto CurrentPlayerHand = PlayerPockets[ActivePlayer];
	for (const auto& CardWidgets : CurrentPlayerHand)
	{
		CardWidgets->bIsHandSelectionOnGoing = false;
		CardWidgets->OnDisableHighlight.Broadcast();
		CardWidgets->OnCardUnselected.Broadcast();
	}
	SelectedHandCard = nullptr;
}


void UGameWidget::ActivateHighlight(EPosition PlayerId, ECardType CardType)
{
	HandCardTypeSelected = CardType;
	
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

	ANAFPlayerController* PlayerController = GetOwningPlayer<ANAFPlayerController>();
	if (!PlayerController) return;
	ANAFPlayerState* PlayerState = PlayerController->GetPlayerState<ANAFPlayerState>();
	if (!PlayerState) return;
	
	const EPosition ActivePlayerId = PlayerState->Id;
	const EPosition OpponentPlayerId = PlayerState->Id == EPosition::LEFT ? EPosition::RIGHT : EPosition::LEFT;

	// Switch 2 cards that are occupied on the same board
	if (CardType == ECardType::SWITCH)
	{
		if (FirstCardSelected == nullptr)
		{
			if (PlayerController->GetNbCardInBoard() >= 2)
			{
				for (auto RowActive : PlayerBoardSlots[ActivePlayerId])
				{
					for (int i = 0; i < RowActive.Num(); i++)
					{
						if (RowActive[i]->bIsCardOccupied)
						{
							RowActive[i]->EnableHighlight();
						}
					}
				}
			}
			if (PlayerController->GetNbCardInOpponentBoard() >= 2)
			{
				for (auto RowActive : PlayerBoardSlots[OpponentPlayerId])
				{
					for (int i = 0; i < RowActive.Num(); i++)
					{
						if (RowActive[i]->bIsCardOccupied)
						{
							RowActive[i]->EnableHighlight();
						}
					}
				}
			}
		}
		else
		{
			DeactivateHighlight();
			
			for (auto RowActive : PlayerBoardSlots[FirstCardPosition])
			{
				for (int i = 0; i < RowActive.Num(); i++)
				{
					if (RowActive[i]->bIsCardOccupied == true)
					{
						RowActive[i]->EnableHighlight();
					}
				}
			}
			FirstCardSelected->SpecialCardFirstChoiceSelected();
		}
		return;
	}
	// Steal one card from opponent or took one from board to place in opponent
	if (CardType == ECardType::STEAL)
	{
		if (FirstCardSelected == nullptr)
		{
			if (PlayerController->GetNbCardInBoard() >= 1)
			{
				for (auto RowActive : PlayerBoardSlots[ActivePlayerId])
				{
					for (int i = 0; i < RowActive.Num(); i++)
					{
						if (RowActive[i]->bIsCardOccupied)
						{
							RowActive[i]->EnableHighlight();
						}
					}
				}
			}
			if (PlayerController->GetNbCardInOpponentBoard() >= 1)
			{
				for (auto RowActive : PlayerBoardSlots[OpponentPlayerId])
				{
					for (int i = 0; i < RowActive.Num(); i++)
					{
						if (RowActive[i]->bIsCardOccupied)
						{
							RowActive[i]->EnableHighlight();
						}
					}
				}
			}
		}
		else
		{
			DeactivateHighlight();
			const EPosition OpposedBoard = FirstCardPosition == EPosition::LEFT ? EPosition::RIGHT : EPosition::LEFT;

			for (auto RowActive : PlayerBoardSlots[OpposedBoard])
			{
				for (int i = 0; i < RowActive.Num(); i++)
				{
					if (RowActive[i]->bIsCardOccupied == false)
					{
						RowActive[i]->EnableHighlight();
						break;
					}
				}
			}
			FirstCardSelected->SpecialCardFirstChoiceSelected();
		}
		return;
	}
	
	if (CardType == ECardType::COPY)
	{
		if (FirstCardSelected == nullptr)
		{
			if (PlayerController->GetNbCardInBoard() == 0) return;
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
			FirstCardSelected->SpecialCardFirstChoiceSelected();
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

void UGameWidget::SwitchTexture(EPosition PlayerId, uint8 HandSlotIndex, UTexture2D* CardTexture)
{
	GetCardWidget(PlayerId, HandSlotIndex)->SwitchTexture(CardTexture);
}

void UGameWidget::UpdateScores(int32 PLeftScore0, int32 PLeftScore1, int32 PLeftScore2, int32 PLeftTotalScore,
	int32 PRightScore0, int32 PRightScore1, int32 PRightScore2, int32 PRightTotalScore)
{
	WBP_ScoreP1_0->UpdateScoreText(PLeftScore0);
	WBP_ScoreP1_1->UpdateScoreText(PLeftScore1);
	WBP_ScoreP1_2->UpdateScoreText(PLeftScore2);
	WBP_ScoreP1_Total->UpdateScoreText(PLeftTotalScore);
	WBP_ScoreP2_0->UpdateScoreText(PRightScore0);
	WBP_ScoreP2_1->UpdateScoreText(PRightScore1);
	WBP_ScoreP2_2->UpdateScoreText(PRightScore2);
	WBP_ScoreP2_Total->UpdateScoreText(PRightTotalScore);
}

void UGameWidget::FamilyEffect(uint8 PlayerId, uint8 Line)
{
	if (PlayerId == 1)
	{
		BoardSlots[Line][0]->CardDestroyByFamEffect();
		BoardSlots[Line][1]->CardDestroyByFamEffect();
		BoardSlots[Line][2]->CardDestroyByFamEffect();
	}
	else
	{
		BoardSlots[Line][3]->CardDestroyByFamEffect();
		BoardSlots[Line][4]->CardDestroyByFamEffect();
		BoardSlots[Line][5]->CardDestroyByFamEffect();
	}
	LineDestroyed.Broadcast(PlayerId, Line);
}

void UGameWidget::NumEffect(TArray<FIntPoint> CoordCardsDeleted)
{
	for (FIntPoint Coord : CoordCardsDeleted)
	{
		BoardSlots[Coord.X][Coord.Y]->CardDestroyedByNumEffect();
	}
}

void UGameWidget::AnimPlaceCardInHand(EPosition PlayerId, uint8 CardPos)
{
	UE_LOG(LogTemp, Warning, TEXT("GetCard call %s %d"), *EnumHelper::ToString(PlayerId), CardPos);
	SendCardToHand.Broadcast(PlayerId, CardPos);
}

void UGameWidget::LaunchAnimPutCard(uint8 Line, uint8 Col)
{
	BoardSlots[Line][Col]->OnPutCardAnim.Broadcast();
}

void UGameWidget::LaunchAnimCombo2(const TArray<FIntPoint> CoordCardsCombo)
{
	for (FIntPoint Coord : CoordCardsCombo)
	{
		BoardSlots[Coord.X][Coord.Y]->OnCombox2Anim.Broadcast();
	}
}

void UGameWidget::LaunchAnimCombo3(const TArray<FIntPoint> CoordCardsCombo)
{
	for (FIntPoint Coord : CoordCardsCombo)
	{
		BoardSlots[Coord.X][Coord.Y]->OnCombox3Anim.Broadcast();
	}
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
		for (int j = 0; j < 6; j++)
		{
			BoardSlots[i][j]->SetSlotParams(j < 3, i, j);
		}
	}

	TArray<TArray<TObjectPtr<UCardWidget>>> PlayerLeftBoardSlots;
	PlayerLeftBoardSlots.SetNum(3);
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
	WBP_Card_P1_Pocket->HideCard();
	WBP_Card_P1_Pocket_1->SetSlotParams(true, 3, 2);
	WBP_Card_P1_Pocket_1->HideCard();
	WBP_Card_P2_Pocket->SetSlotParams(false, 3, 3);
	WBP_Card_P2_Pocket->HideCard();
	WBP_Card_P2_Pocket_1->SetSlotParams(false, 3, 4);
	WBP_Card_P2_Pocket_1->HideCard();
	
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
				BoardSlots[i][j]->OnClickSlot.AddDynamic(this, &UGameWidget::OnBoardCardSelected);
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
				PocketCard->OnClickSlot.AddDynamic(this, &UGameWidget::OnHandCardSelected);
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
			DeactivateHighlight();
			if (HandCardTypeSelected == ECardType::COPY) OnShowCopyCardInHand.Broadcast(9,9);
			
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
				HandCardTypeSelected = ECardType::NONE;
				FirstCardSelected = nullptr;
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
	if (LineSelect < 3 && ColSelect < 6)
	{
		if (FirstCardSelected == nullptr)
		{
			FirstCardSelected = BoardSlots[LineSelect][ColSelect];
			FirstCardSelected->SpecialCardFirstChoiceSelected();
			FirstCardPosition = Player;
		}
		else
		{
			// if special card and first card is the same => cancel action
			if (FirstCardSelected && FirstCardSelected->Line == LineSelect && FirstCardSelected->Col == ColSelect)
			{
				if (HandCardTypeSelected == ECardType::COPY) OnShowCopyCardInHand.Broadcast(9,9);
				
				FirstCardSelected = nullptr;				
				DeactivateHighlight();
				ActivateHighlight(GetPlayerId(),HandCardTypeSelected);
				return;
			}

			SecondCardSelected = BoardSlots[LineSelect][ColSelect];
			SecondCardSelected->SpecialCardFirstChoiceSelected();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Wrong slot coordinates, received (%d,%d)"), LineSelect, ColSelect);
		return;
	}
	
	// if normal card
	if (HandCardTypeSelected == ECardType::NORMAL)
	{
		OnClickBoardSlot.Broadcast(LineSelect, ColSelect);
		FirstCardSelected = nullptr;
		SecondCardSelected = nullptr;
		return;
	}

	if (HandCardTypeSelected == ECardType::COPY)
	{
		if (FirstCardSelected && !SecondCardSelected)
		{
			// Show Copy on special card
			OnShowCopyCardInHand.Broadcast(FirstCardSelected->Line, FirstCardSelected->Col);
			// Broadcast show card copy
			ActivateHighlight(GetPlayerId(), HandCardTypeSelected);
			return;
		}
		if (FirstCardSelected && SecondCardSelected)
		{
			OnActiveCopy.Broadcast(FirstCardSelected->OwningPlayer, FirstCardSelected->Line, FirstCardSelected->Col,
									SecondCardSelected->OwningPlayer, SecondCardSelected->Line, SecondCardSelected->Col);
			FirstCardSelected = nullptr;
			SecondCardSelected = nullptr;
			HandCardTypeSelected = ECardType::NONE;
			return;
		}
	}
	if (HandCardTypeSelected == ECardType::SWITCH)
	{
		if (FirstCardSelected && !SecondCardSelected)
		{
			ActivateHighlight(GetPlayerId(), HandCardTypeSelected);
			return;
		}
		if (FirstCardSelected && SecondCardSelected)
		{
			OnActiveSwitch.Broadcast(FirstCardSelected->OwningPlayer, FirstCardSelected->Line, FirstCardSelected->Col,
									SecondCardSelected->OwningPlayer, SecondCardSelected->Line, SecondCardSelected->Col);
			FirstCardSelected = nullptr;
			SecondCardSelected = nullptr;
			return;
		}
	}
	if (HandCardTypeSelected == ECardType::STEAL)
	{
		if (FirstCardSelected && !SecondCardSelected)
		{
			ActivateHighlight(GetPlayerId(), HandCardTypeSelected);
			return;
		}
		if (FirstCardSelected && SecondCardSelected)
		{
			OnActiveSteal.Broadcast(FirstCardSelected->OwningPlayer, FirstCardSelected->Line, FirstCardSelected->Col,
									SecondCardSelected->OwningPlayer, SecondCardSelected->Line, SecondCardSelected->Col);
			FirstCardSelected = nullptr;
			SecondCardSelected = nullptr;
			return;
		}
	}
}

EPosition UGameWidget::GetPlayerId()
{
	if (ANAFPlayerController* PlayerController = GetOwningPlayer<ANAFPlayerController>())
	{
		return PlayerController->GetPlayerState<ANAFPlayerState>()->Id;
	}
	return EPosition::SERVER;
}

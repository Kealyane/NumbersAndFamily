// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameWidget.h"

#include "NumbersAndFamily/NumbersAndFamily.h"
#include "Widgets/CardWidget.h"
#include "Widgets/ScoreWidget.h"
#include "Components/TextBlock.h"
#include "Player/NAFPlayerController.h"
#include "Player/NAFPlayerState.h"


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
	//GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Turquoise,
	//FString::Printf(TEXT("GameWidget : active highlight id %s card %s"), *EnumHelper::ToString(PlayerId), *EnumCardTypeHelper::ToString(CardType)));
	UE_LOG(LogTemp, Warning, TEXT("GameWidget : active highlight id %s card %s"), *EnumHelper::ToString(PlayerId), *EnumCardTypeHelper::ToString(CardType));

	HandCardTypeSelected = CardType;
	UE_LOG(LogTemp, Warning, TEXT("GameWidget : active highlight init HandCardTypeSelected (%s)"), *EnumCardTypeHelper::ToString(HandCardTypeSelected));
	
	if (CardType == ECardType::NORMAL)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameWidget : active highlight Normal (%s)"), *EnumCardTypeHelper::ToString(CardType));
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
		UE_LOG(LogTemp, Warning, TEXT("GameWidget : active highlight Switch (%s)"), *EnumCardTypeHelper::ToString(CardType));
		if (FirstCardSelected == nullptr)
		{
			if (PlayerController->GetNbCardInBoard() >= 2)
			{
				UE_LOG(LogTemp, Warning, TEXT("GameWidget : active highlight First Card to pick - Current Board has enough card"));
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
				UE_LOG(LogTemp, Warning, TEXT("GameWidget : active highlight First Card to pick - Opponent Board has enough card"));
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
			UE_LOG(LogTemp, Warning, TEXT("GameWidget : active highlight Second Card to pick"));
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
		UE_LOG(LogTemp, Warning, TEXT("GameWidget : active highlight Steal (%s)"), *EnumCardTypeHelper::ToString(CardType));
		if (FirstCardSelected == nullptr)
		{
			if (PlayerController->GetNbCardInBoard() >= 1)
			{
				UE_LOG(LogTemp, Warning, TEXT("GameWidget : active highlight First Card to pick - Current Board has enough card"));
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
				UE_LOG(LogTemp, Warning, TEXT("GameWidget : active highlight First Card to pick - Opponent Board has enough card"));
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
			UE_LOG(LogTemp, Warning, TEXT("GameWidget : active highlight Second Card to pick"));
			DeactivateHighlight();
			const EPosition OpposedBoard = FirstCardPosition == EPosition::LEFT ? EPosition::RIGHT : EPosition::LEFT;

			for (auto RowActive : PlayerBoardSlots[OpposedBoard])
			{
				for (int i = 0; i < RowActive.Num(); i++)
				{
					if (RowActive[i]->bIsCardOccupied == false)
					{
						RowActive[i]->EnableHighlight();
					}
				}
			}
			FirstCardSelected->SpecialCardFirstChoiceSelected();
		}
		return;
	}
	
	if (CardType == ECardType::COPY)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameWidget : active highlight Copy (%s)"), *EnumCardTypeHelper::ToString(CardType));
		if (FirstCardSelected == nullptr)
		{
			if (PlayerController->GetNbCardInBoard() == 0) return;
			UE_LOG(LogTemp, Warning, TEXT("GameWidget : active highlight first Card to pick"));
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
			UE_LOG(LogTemp, Warning, TEXT("GameWidget : active highlight Second Card to pick"));
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
	// GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Turquoise,
	// FString::Printf(TEXT("GameWidget : ActiveHandHighlight id %s"), *EnumHelper::ToString(PlayerId)));
	
	auto CurrentPlayerHand = PlayerPockets[PlayerId];
	for (const auto& CardWidgets : CurrentPlayerHand)
	{
		CardWidgets->EnableHighlight();	
	}
}

void UGameWidget::DeactivateHandHighlight(EPosition PlayerId)
{
// 	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Turquoise,
// FString::Printf(TEXT("GameWidget : DeactivateHandHighlight id %s"), *EnumHelper::ToString(PlayerId)));
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
		for (int j = 0; j < 6; j++)
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
			DeactivateHighlight();
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
			UE_LOG(LogTemp, Warning, TEXT("Game Widget : OnBoardCardSelected = assign FirstCardSelected"));
		}
		else
		{
			// if special card and first card is the same => cancel action
			if (FirstCardSelected && FirstCardSelected->Line == LineSelect && FirstCardSelected->Col == ColSelect)
			{
				UE_LOG(LogTemp, Warning, TEXT("Game Widget : OnBoardCardSelected = click FirstCardSelected => cancel"));

				FirstCardSelected = nullptr;				
				DeactivateHighlight();
				ActivateHighlight(GetPlayerId(),HandCardTypeSelected);
				return;
			}
			UE_LOG(LogTemp, Warning, TEXT("Game Widget : OnBoardCardSelected = assign SecondCardSelected"));
			SecondCardSelected = BoardSlots[LineSelect][ColSelect];
			SecondCardSelected->SpecialCardFirstChoiceSelected();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Wrong slot coordinates, received (%d,%d)"), LineSelect, ColSelect);
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Game Widget : OnBoardCardSelected = HandCardTypeSelected (%s)"), *EnumCardTypeHelper::ToString(HandCardTypeSelected));
	
	// if normal card
	if (HandCardTypeSelected == ECardType::NORMAL)
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Widget : OnBoardCardSelected = Broadcast play normal card (%d,%d)"), LineSelect, ColSelect);
		OnClickBoardSlot.Broadcast(LineSelect, ColSelect);
		FirstCardSelected = nullptr;
		SecondCardSelected = nullptr;
		return;
	}

	if (HandCardTypeSelected == ECardType::COPY)
	{
		if (FirstCardSelected && !SecondCardSelected)
		{
			UE_LOG(LogTemp, Warning, TEXT("Game Widget : OnBoardCardSelected COPY = Highlight second selection"));
			// Show Copy on special card
			// Broadcast show card copy
			ActivateHighlight(GetPlayerId(), HandCardTypeSelected);
			return;
		}
		if (FirstCardSelected && SecondCardSelected)
		{
			UE_LOG(LogTemp, Warning, TEXT("Game Widget : OnBoardCardSelected COPY = Broadcast play COPY card"));
			// Broadcast Play Copy
			FirstCardSelected = nullptr;
			SecondCardSelected = nullptr;
			return;
		}
	}
	if (HandCardTypeSelected == ECardType::SWITCH)
	{
		if (FirstCardSelected && !SecondCardSelected)
		{
			UE_LOG(LogTemp, Warning, TEXT("Game Widget : OnBoardCardSelected SWITCH = Highlight second selection"));
			ActivateHighlight(GetPlayerId(), HandCardTypeSelected);
			return;
		}
		if (FirstCardSelected && SecondCardSelected)
		{
			UE_LOG(LogTemp, Warning, TEXT("Game Widget : OnBoardCardSelected SWITCH = Broadcast play SWITCH card"));
			// Broadcast Play Switch
			FirstCardSelected = nullptr;
			SecondCardSelected = nullptr;
			return;
		}
	}
	if (HandCardTypeSelected == ECardType::STEAL)
	{
		if (FirstCardSelected && !SecondCardSelected)
		{
			UE_LOG(LogTemp, Warning, TEXT("Game Widget : OnBoardCardSelected STEAL = Highlight second selection"));
			ActivateHighlight(GetPlayerId(), HandCardTypeSelected);
			return;
		}
		if (FirstCardSelected && SecondCardSelected)
		{
			UE_LOG(LogTemp, Warning, TEXT("Game Widget : OnBoardCardSelected STEAL = Broadcast play STEAL card"));
			// Broadcast Play Steal
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

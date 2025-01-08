// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NAFPlayerController.h"

#include "SoundNames.h"
#include "Blueprint/UserWidget.h"
#include "GameElements/AudioContainer.h"
#include "GameElements/Deck.h"
#include "GameMode/NAFGameMode.h"
#include "GameMode/NAFGameState.h"
#include "Player/NAFPlayerState.h"
#include "Widgets/CardWidget.h"
#include "Widgets/EndGameWidget.h"
#include "Widgets/GameWidget.h"


void ANAFPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController())
	{
		if (UWorld* World = GetWorld())
		{
			AudioManager = World->SpawnActor<AAudioContainer>(AudioContainerClass);
		}
	}
}

void ANAFPlayerController::ClientRPC_ShowGameBoard_Implementation()
{
	GameWidget = CreateWidget<UGameWidget>(this, GameWidgetType);
	if (GameWidget)
	{
		GameWidget->AddToViewport();
		GameWidget->OnSendHandCardSelected.AddDynamic(this, &ANAFPlayerController::GetCardTypeSelected);
		GameWidget->OnClickBoardSlot.AddDynamic(this, &ANAFPlayerController::GetSelectedHandCard);
		GameWidget->OnActiveSwitch.AddDynamic(this, &ANAFPlayerController::HandleSwitch);
		GameWidget->OnActiveSteal.AddDynamic(this, &ANAFPlayerController::HandleSteal);
		GameWidget->OnShowCopyCardInHand.AddDynamic(this, &ANAFPlayerController::ShowCopyCardInHand);
		GameWidget->OnActiveCopy.AddDynamic(this, &ANAFPlayerController::HandleCopy);
		GameWidget->SetIsFocusable(true);
	}
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(GameWidget->TakeWidget()); 
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	SetInputMode(InputMode);
	
}

void ANAFPlayerController::ClientRPC_PlaceCardInPocketUI_Implementation(EPosition PlayerPosition, uint8 Pos,
	FName CardRowName)
{
	const FString ContextString(TEXT("Card Data Context"));
	const FCardData* Data = DeckDataTable->FindRow<FCardData>(CardRowName,ContextString);
	
	if (GameWidget && Data)
	{
		AudioManager->PlayAudio(ESoundRow::CardDraw);
		GameWidget->AnimPlaceCardInHand(PlayerPosition, Pos);
		GetWorld()->GetTimerManager().SetTimer(PlaceCardHandle,
			[this, PlayerPosition, Pos, Data]()
			{
				UCardWidget* CardSlot = GameWidget->GetCardWidget(PlayerPosition, Pos);
				if (CardSlot)
				{
					CardSlot->ShowCardAnim();
	 				CardSlot->ShowCard(Data->ImageRecto);
				}
			},
			1.5f,
			false);
	}
	
}

void ANAFPlayerController::ClientRPC_ShowPocketCardVerso_Implementation(EPosition PlayerPosition, uint8 Pos)
{
	if (GameWidget)
	{
		UCardWidget* CardUW = GameWidget->GetCardWidget(PlayerPosition, Pos);
		if (CardUW)
		{
			CardUW->ShowCard(CardVerso);
		}
	}
}

void ANAFPlayerController::ClientRPC_PocketCardEmpty_Implementation(EPosition PlayerPosition, uint8 Pos)
{
	if (GameWidget)
	{
		GameWidget->GetCardWidget(PlayerPosition, Pos)->HideCard();
	}
}

void ANAFPlayerController::ShowEndGamePanel(EPosition WinningPlayerId)
{
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	if (!NafPlayerState) return;
	
	EndGameWidget = CreateWidget<UEndGameWidget>(this, EndGameWidgetType);
	if (EndGameWidget)
	{
		EndGameWidget->AddToViewport(5);
		EndGameWidget->SetIsFocusable(true);
		EndGameWidget->SetEndGameText(NafPlayerState->Id==WinningPlayerId, NafPlayerState->GetTotalScore());
	}
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(EndGameWidget->TakeWidget()); 
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	SetInputMode(InputMode);
}

void ANAFPlayerController::ShowCard(FCardDataServer Card, uint8 Line, uint8 col)
{
	const FString ContextString(TEXT("Card Data Context"));
	const FCardData* Data = DeckDataTable->FindRow<FCardData>(Card.RowName,ContextString);
	if (Data && GameWidget)
	{
		GameWidget->BoardSlots[Line][col]->ShowCard(Data->ImageRecto);
	}
}

void ANAFPlayerController::ServerRPC_DrawCard_Implementation()
{
	if (ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->DrawCard(GetPlayerState<ANAFPlayerState>());
	}
}

bool ANAFPlayerController::ServerRPC_DrawCard_Validate()
{
	return IsActivePlayer();
}

void ANAFPlayerController::ClientRPC_ActiveHand_Implementation(EPosition PlayerPosition)
{
	GameWidget->StartHandSelection(PlayerPosition);
	GameWidget->ActiveHandHighlight(PlayerPosition);
}


void ANAFPlayerController::ServerRPC_PlaceNormalCard_Implementation(FCardDataServer Card, uint8 IndexHandCard, uint8 Line, uint8 Col)
{
	if (ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->PlaceNormalCard(Card, IndexHandCard, Line, Col);
	}
}


bool ANAFPlayerController::ServerRPC_PlaceNormalCard_Validate(FCardDataServer Card, uint8 IndexHandCard, uint8 Line, uint8 Col)
{
	if (Card.FamilyType == EFamilyType::NONE || IndexHandCard == 0) return false;
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	return IsActivePlayer() && IsCoordInPlayerIdSide(NafPlayerState->Id, Line, Col);
}

void ANAFPlayerController::ServerRPC_EndTurn_Implementation(ANAFPlayerState* ActivePlayerState)
{
	if (!IsActivePlayer()) return;
	
	if (ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->RemoveCardFromHand(ActivePlayerState);
		FTimerHandle ClearHandHandle;
		GetWorld()->GetTimerManager().SetTimer(ClearHandHandle,
			[this, GameMode]()
			{
				GameMode->EndTurn();
			},
			1.f, false);
	}
}


void ANAFPlayerController::ServerRPC_ActiveSwitch_Implementation(uint8 IndexHandCard, EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col,
	EPosition Card2Pos, uint8 Card2Line, uint8 Card2Col)
{
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	if (!NafPlayerState) return;
	
	if (ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->SwitchCardsInBoard(IndexHandCard, Card1Line, Card1Col, Card2Line, Card2Col);
	}
}

bool ANAFPlayerController::ServerRPC_ActiveSwitch_Validate(uint8 IndexHandCard, EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col,
	EPosition Card2Pos, uint8 Card2Line, uint8 Card2Col)
{
	ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode == nullptr) return false;
	
	return Card1Pos == Card2Pos &&
		IsCoordInPlayerIdSide(Card1Pos, Card1Line,  Card1Col) &&
		IsCoordInPlayerIdSide(Card2Pos, Card2Line, Card2Col) &&
		GameMode->IsCoordOccupiedInBoard(Card1Line, Card1Col) &&
		GameMode->IsCoordOccupiedInBoard(Card2Line, Card2Col);
}

void ANAFPlayerController::ServerRPC_ActiveSteal_Implementation(uint8 IndexHandCard, EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col,
	EPosition Card2Pos, uint8 Card2Line, uint8 Card2Col)
{
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	if (!NafPlayerState) return;
	
	if (ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->StealCardInBoard(IndexHandCard, Card1Line, Card1Col, Card2Line, Card2Col);
	}
}

bool ANAFPlayerController::ServerRPC_ActiveSteal_Validate(uint8 IndexHandCard, EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col,
	EPosition Card2Pos, uint8 Card2Line, uint8 Card2Col)
{
	ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode == nullptr) return false;

	return Card1Pos != Card2Pos &&
		IsCoordInPlayerIdSide(Card1Pos, Card1Line, Card1Col) &&
		IsCoordInPlayerIdSide(Card2Pos, Card2Line, Card2Col) &&
		GameMode->IsCoordOccupiedInBoard(Card1Line, Card1Col) &&
		!GameMode->IsCoordOccupiedInBoard(Card2Line, Card2Col);
}

void ANAFPlayerController::ServerRPC_RequestCardData_Implementation(uint8 Line, uint8 Col)
{
	ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode == nullptr) return;

	FName RowName = GameMode->GetCardDataRowName(Line,Col);
	ClientRPC_ReceiveCardDate(RowName);
}

void ANAFPlayerController::ClientRPC_ReceiveCardDate_Implementation(FName CardRowName)
{
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	if (!NafPlayerState) return;

	if (CardRowName.IsNone())
	{
		GameWidget->SwitchTexture(NafPlayerState->Id, NafPlayerState->GetIndexSelected(),CopyCardTexture);
		return;
	}

	const FString ContextString(TEXT("Card Data Context"));
	const FCardData* Data = DeckDataTable->FindRow<FCardData>(CardRowName,ContextString);
	
	if (Data)
	{
		GameWidget->SwitchTexture(NafPlayerState->Id, NafPlayerState->GetIndexSelected(),Data->ImageRecto);
	}
}

void ANAFPlayerController::ServerRPC_ActiveCopy_Implementation(FCardDataServer Card, uint8 IndexHandCard,
	EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col,
	EPosition Card2Pos, uint8 Card2Line, uint8 Card2Col)
{
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	if (!NafPlayerState) return;
	
	if (ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->CopyCardInBoard(IndexHandCard,
			Card1Line, Card1Col, Card2Line, Card2Col, Card);
	}
}

bool ANAFPlayerController::ServerRPC_ActiveCopy_Validate(FCardDataServer Card, uint8 IndexHandCard,
	EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col,
	EPosition Card2Pos, uint8 Card2Line, uint8 Card2Col)
{
	ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode == nullptr) return false;

	return Card1Pos == Card2Pos &&
	IsCoordInPlayerIdSide(Card1Pos, Card1Line, Card1Col) &&
	IsCoordInPlayerIdSide(Card2Pos, Card2Line, Card2Col) &&
	GameMode->IsCoordOccupiedInBoard(Card1Line, Card1Col) &&
	!GameMode->IsCoordOccupiedInBoard(Card2Line, Card2Col);
}


void ANAFPlayerController::ServerRPC_RowNameForCopy_Implementation(uint8 Line, uint8 Col)
{
	if (ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode()))
	{
		FName Row = GameMode->GetCardDataRowName(Line, Col);
		ClientRPC_RowNameForCopy(Row);
	}
}

void ANAFPlayerController::ClientRPC_RowNameForCopy_Implementation(const FName& RowName)
{
	const FString ContextString(TEXT("Card Data Context"));
	const FCardData* Data = DeckDataTable->FindRow<FCardData>(RowName,ContextString);
	UTexture2D* ImageCard = Data->ImageRecto;
	if (GameWidget) GameWidget->ActiveCopyAnim.Broadcast(ImageCard);
}

void ANAFPlayerController::ServerRPC_RowNameForSteal_Implementation(uint8 Line, uint8 Col)
{
	if (ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode()))
	{
		FName Row = GameMode->GetCardDataRowName(Line, Col);
		ClientRPC_RowNameForSteal(Row);
	}
}

void ANAFPlayerController::ClientRPC_RowNameForSteal_Implementation(const FName& RowName)
{
	const FString ContextString(TEXT("Card Data Context"));
	const FCardData* Data = DeckDataTable->FindRow<FCardData>(RowName,ContextString);
	UTexture2D* ImageCard = Data->ImageRecto;
	if (GameWidget) GameWidget->ActiveStealAnim.Broadcast(ImageCard);
}

void ANAFPlayerController::ServerRPC_RowNameForSwitch_Implementation(uint8 Line1, uint8 Col1, uint8 Line2, uint8 Col2)
{
	if (ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode()))
	{
		FName Row1 = GameMode->GetCardDataRowName(Line1, Col1);
		FName Row2 = GameMode->GetCardDataRowName(Line2, Col2);
		ClientRPC_RowNameForSwitch(Row1, Row2);
	}
}

void ANAFPlayerController::ClientRPC_RowNameForSwitch_Implementation(const FName& RowName1, const FName& RowName2)
{
	const FString ContextString(TEXT("Card Data Context"));
	const FCardData* Data1 = DeckDataTable->FindRow<FCardData>(RowName1,ContextString);
	UTexture2D* ImageCard1 = Data1->ImageRecto;
	const FCardData* Data2 = DeckDataTable->FindRow<FCardData>(RowName2,ContextString);
	UTexture2D* ImageCard2 = Data2->ImageRecto;
	if (GameWidget) GameWidget->ActiveSwitchAnim.Broadcast(ImageCard1, ImageCard2);
}

void ANAFPlayerController::UpdateActiveTurnUI(EPosition ActivePosition)
{
	if (GameWidget)
	{
		GameWidget->ShowActivePlayer(ActivePosition);
	}
}

void ANAFPlayerController::NotifyTurnStart(bool bInIsMyTurn)
{
	bIsMyTurn = bInIsMyTurn;
	if (bIsMyTurn)
	{
		ServerRPC_DrawCard();
	}
}

void ANAFPlayerController::EndTurn()
{
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	ANAFGameState* GameState = GetWorld()->GetGameState<ANAFGameState>();

	if (GameState && NafPlayerState && NafPlayerState->Id == GameState->ActiveId)
	{
		GameWidget->EndHandSelection(NafPlayerState->Id);
		GameWidget->DeactivateHandHighlight(NafPlayerState->Id);
		GameWidget->DeactivateHighlight();
		
		GetWorld()->GetTimerManager().SetTimer(EndTurnHandle,
			[this, NafPlayerState]()
			{
				ServerRPC_EndTurn(NafPlayerState);
			}
			, 2.f, false);
	}
}

void ANAFPlayerController::EnableCardSelectionUI(EPosition PlayerId, ECardType CardType)
{
	GameWidget->ActivateHighlight(PlayerId, CardType);
}

void ANAFPlayerController::GetCardTypeSelected(uint8 PosInHand)
{
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	if (!IsActivePlayer() && !NafPlayerState) return;
	
	ECardType CardType = NafPlayerState->GetCardType(PosInHand);

	if (CardType != ECardType::NONE)
	{
		GameWidget->ActivateHighlight(NafPlayerState->Id, CardType);
	}
	else
	{
		GameWidget->ActiveHandHighlight(NafPlayerState->Id);
		GameWidget->DeactivateHighlight();
	}
}

void ANAFPlayerController::GetSelectedHandCard(uint8 Line, uint8 Col)
{
	if (!IsActivePlayer()) return;
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();

	if (GameWidget)
	{
		GameWidget->GetCardWidget(NafPlayerState->Id, NafPlayerState->GetIndexSelected())->HideCard();
	}

	if (NafPlayerState)
	{
		ServerRPC_PlaceNormalCard(NafPlayerState->GetSelectedCard(), NafPlayerState->GetIndexSelected(), Line, Col);
	}
}

void ANAFPlayerController::HandleSwitch(EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col, EPosition Card2Pos,
	uint8 Card2Line, uint8 Card2Col)
{
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	if (!NafPlayerState) return;

	ServerRPC_RowNameForSwitch(Card1Line, Card1Col, Card2Line, Card2Col);
	
	FTimerHandle SpecialCardHandle;
	float AnimTime = 2.2f;
	GetWorld()->GetTimerManager().SetTimer(SpecialCardHandle,
	[this, NafPlayerState, Card1Pos, Card1Line, Card1Col, Card2Pos, Card2Line, Card2Col]()
	{
		ServerRPC_ActiveSwitch(NafPlayerState->GetIndexSelected(), Card1Pos, Card1Line, Card1Col, Card2Pos, Card2Line, Card2Col);
	},
	AnimTime,
	false);
}

void ANAFPlayerController::HandleSteal(EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col, EPosition Card2Pos,
	uint8 Card2Line, uint8 Card2Col)
{
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	if (!NafPlayerState) return;

	ServerRPC_RowNameForSteal(Card1Line, Card1Col);
	
	FTimerHandle SpecialCardHandle;
	float AnimTime = 2.2f;
	GetWorld()->GetTimerManager().SetTimer(SpecialCardHandle,
	[this, NafPlayerState, Card1Pos, Card1Line, Card1Col, Card2Pos, Card2Line, Card2Col]()
	{
		ServerRPC_ActiveSteal(NafPlayerState->GetIndexSelected(), Card1Pos, Card1Line, Card1Col, Card2Pos, Card2Line, Card2Col);
	},
	AnimTime,
	false);
}

void ANAFPlayerController::ShowCopyCardInHand(uint8 Line, uint8 Col)
{
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	if (!NafPlayerState) return;
	
	if (Line == 9)
	{
		GameWidget->SwitchTexture(NafPlayerState->Id, NafPlayerState->GetIndexSelected(), CopyCardTexture);
	}
	else
	{
		ServerRPC_RequestCardData(Line, Col);
	}
}

void ANAFPlayerController::HandleCopy(EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col, EPosition Card2Pos,
	uint8 Card2Line, uint8 Card2Col)
{
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();

	ServerRPC_RowNameForCopy(Card1Line,Card1Col);

	FTimerHandle SpecialCardHandle;
	float AnimTime = 2.2f;
	GetWorld()->GetTimerManager().SetTimer(SpecialCardHandle,
	[this, NafPlayerState, Card1Pos, Card1Line, Card1Col, Card2Pos, Card2Line, Card2Col]()
	{
		ServerRPC_ActiveCopy(NafPlayerState->GetSelectedCard(),NafPlayerState->GetIndexSelected(),
			Card1Pos, Card1Line, Card1Col,
			Card2Pos, Card2Line, Card2Col);
	},
	AnimTime,
	false);
}

void ANAFPlayerController::FamilyEffect(uint8 PlayerId, uint8 Line)
{
	if (GameWidget)
	{
		GameWidget->FamilyEffect(PlayerId, Line);
	}
}

void ANAFPlayerController::NumEffect(TArray<FIntPoint> CoordCardsDeleted)
{
	if (GameWidget)
	{
		GameWidget->NumEffect(CoordCardsDeleted);
	}
}

void ANAFPlayerController::UpdateBoardCard(bool bAfterPlayerAction, const TArray<FName>& InBoardTableRow)
{

	
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	if (!NafPlayerState) return;

	NbCardInBoard = 0;
	NBCardInOpponentBoard = 0;
	
	const FString ContextString(TEXT("Card Data Context"));
	for (int i = 0; i < InBoardTableRow.Num(); i++)
	{
		int row = i / 6;
		int col = i % 6;
		if (row >= 0 && row < 3 && col >= 0 && col < 6)
		{
			if (UCardWidget* CardSlot = GameWidget->BoardSlots[row][col])
			{
				if (InBoardTableRow[i].IsNone())
				{
					CardSlot->HideCard();
				}
				else
				{
					const FCardData* Data = DeckDataTable->FindRow<FCardData>(InBoardTableRow[i],ContextString);
					if (GameWidget && Data)
					{
						CardSlot->ShowCard(Data->ImageRecto);
						
						if (IsCoordInPlayerIdSide(NafPlayerState->Id, row, col))
						{
							NbCardInBoard++; 
						}
						else
						{
							NBCardInOpponentBoard++;
						}
					}
				}
			}
		}
	}
	if (bAfterPlayerAction)
	{
		EndTurn();
	}
}

void ANAFPlayerController::UpdateScores(int32 PLeftScore0, int32 PLeftScore1, int32 PLeftScore2, int32 PLeftTotalScore,
	int32 PRightScore0, int32 PRightScore1, int32 PRightScore2, int32 PRightTotalScore)
{
	if (GameWidget)
	{
		GameWidget->UpdateScores(PLeftScore0, PLeftScore1, PLeftScore2, PLeftTotalScore,
			PRightScore0, PRightScore1, PRightScore2, PRightTotalScore);
	}
}

void ANAFPlayerController::LaunchAnimPutCard(uint8 Line, uint8 Col)
{
	if (GameWidget)
	{
		GameWidget->LaunchAnimPutCard(Line, Col);
	}
}

void ANAFPlayerController::LaunchAnimCombo2(const TArray<FIntPoint> CoordCardsCombo)
{
	if (GameWidget)
	{
		GameWidget->LaunchAnimCombo2(CoordCardsCombo);
	}
}

void ANAFPlayerController::LaunchAnimCombo3(const TArray<FIntPoint> CoordCardsCombo)
{
	if (GameWidget)
	{
		GameWidget->LaunchAnimCombo3(CoordCardsCombo);
	}
}

void ANAFPlayerController::SetPlayerNames(FName Player1, FName Player2)
{
	if (GameWidget)
	{
		GameWidget->SetPlayer1Name.Broadcast(Player1);
		GameWidget->SetPlayer2Name.Broadcast(Player2);
	}
}


bool ANAFPlayerController::IsCoordInPlayerIdSide(EPosition PlayerId, uint8 Line, uint8 Col)
{
	if (PlayerId == EPosition::LEFT)
	{
		return Line < 3 && Col < 3;
	}
	return Line < 3 && Col >= 3 && Col < 6;
}

bool ANAFPlayerController::IsActivePlayer() const
{
	ANAFGameState* GameState = GetWorld()->GetGameState<ANAFGameState>();
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();

	return GameState && NafPlayerState && NafPlayerState->Id == GameState->ActiveId;
}

EPosition ANAFPlayerController::GetPlayerId() const
{
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	if (!NafPlayerState) return EPosition::SERVER;
	return NafPlayerState->Id;
}



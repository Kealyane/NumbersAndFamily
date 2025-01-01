// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NAFGameMode.h"

#include "GameElements/Board.h"
#include "GameFramework/GameState.h"
#include "GameMode/NAFGameState.h"
#include "Player/NAFPlayerController.h"
#include "Player/NAFPlayerState.h"
#include "GameElements/Deck.h"

void ANAFGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState->PlayerArray.Num();

	if (ANAFPlayerController* TPC = Cast<ANAFPlayerController>(NewPlayer))
	{
		if (ANAFPlayerState* PS = Cast<ANAFPlayerState>(TPC->PlayerState))
		{
			if (NumberOfPlayers == 1)
			{
				PS->Id = EPosition::LEFT;
			}
			else if (NumberOfPlayers == 2)
			{
				PS->Id = EPosition::RIGHT;
			}
		}
	}
	
	if (NumberOfPlayers == 2)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(WaitHandle, this, &ANAFGameMode::LaunchGame, 1.f, false);
		}
	}
}

void ANAFGameMode::LaunchGame()
{
	bIsGameOver = false;
	Winner = EPosition::SERVER;

	UWorld* World = GetWorld();
	
	if (World)
	{
		World->GetTimerManager().ClearTimer(WaitHandle);
	}

	NafGameState = GetGameState<ANAFGameState>();
	
	// Link to Level Blueprint to remove lobby widget
	if (NafGameState)
	{
		NafGameState->OnLeaveLobby.Broadcast();
	}

	// Show UI
	for (FConstPlayerControllerIterator PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; ++PCIterator)
	{
		if (ANAFPlayerController* NafPlayerController = Cast<ANAFPlayerController>(*PCIterator))
		{
			NafPlayerController->ClientRPC_ShowGameBoard();
		}
	}

	// Initialize Deck
	if (World)
	{
		Deck = World->SpawnActor<ADeck>(DeckClassType);
		if (!Deck->InitDeck()) return;
		Board = World->SpawnActor<ABoard>(BoardClassType);
		Board->InitBoard();
		Board->InitParamDeck(Deck);
		NafGameState->InitBoardRow();
	

		NafGameState->MultiRPC_PlaySoundForBoth(ESoundRow::StartGame);


		DelayCounter += 1.5f;
		World->GetTimerManager().SetTimer(ShuffleHandle,
			[this,World]()
			{
				NafGameState->MultiRPC_PlaySoundForBoth(ESoundRow::Shuffle);
			},
			DelayCounter,
			false);
		
		DelayCounter += 3.f;
		World->GetTimerManager().SetTimer(InitCardHandle,this,&ANAFGameMode::GiveOneCardToBothPlayer, DelayCounter, false);
		
		DelayCounter += 1.f;
		World->GetTimerManager().SetTimer(SwitchPlayerHandle,this,&ANAFGameMode::InitializeCurrentPlayer, DelayCounter, false);
	}
	
}

void ANAFGameMode::EndGame()
{
	if (NafGameState)
	{
		NafGameState->MultiRPC_EndGame(Winner);
	}
}

void ANAFGameMode::DrawCard(ANAFPlayerState* ActivePlayerState)
{
	if (bIsGameOver)
	{
		EndGame();
		return;
	}

	FCardDataServer Card = Deck->DrawCard();
	if (Card.RowName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode : Can't draw card, empty"));
		return;
	}

	ActivePlayerState->StoreCardInHand(Card);
	ActivePlayerState->ActiveHandChoice(ActivePlayerState->Id);
	TArray<bool> HandCurrent = ActivePlayerState->HandStatus();
	ANAFPlayerState* OpponentPS = NafGameState->GetOpponentPlayerState(ActivePlayerState->Id);
	OpponentPS->UpdateHandUI(ActivePlayerState->Id,HandCurrent);
}

void ANAFGameMode::RemoveCardFromHand(ANAFPlayerState* ActivePlayerState)
{
	ActivePlayerState->RemoveCardInHand(IndexHandCardPlayed);
	TArray<bool> HandCurrent = ActivePlayerState->HandStatus();
	ANAFPlayerState* OpponentPS = NafGameState->GetOpponentPlayerState(ActivePlayerState->Id);
	OpponentPS->UpdateHandUI(ActivePlayerState->Id,HandCurrent);
}

void ANAFGameMode::PlaceNormalCard(FCardDataServer Card, uint8 IndexHandCard, uint8 Line, uint8 Col)
{
	IndexHandCardPlayed = IndexHandCard;
	Board->PlaceNormalCard(Card, Line, Col);
}

void ANAFGameMode::EndTurn()
{
	NafGameState->SwitchPlayerTurn();
}

bool ANAFGameMode::IsCoordOccupiedInBoard(uint8 Line, uint8 Col)
{
	return Board->IsCoordOccupied(Line, Col);
}

void ANAFGameMode::SwitchCardsInBoard(uint8 IndexHandCard, uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col)
{
	IndexHandCardPlayed = IndexHandCard;
	Board->SwitchCard(Card1Line, Card1Col, Card2Line, Card2Col);
}

void ANAFGameMode::StealCardInBoard(uint8 IndexHandCard, uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col)
{
	IndexHandCardPlayed = IndexHandCard;
	Board->StealCard(Card1Line, Card1Col, Card2Line, Card2Col);
}

FName ANAFGameMode::GetCardDataRowName(uint8 Line, uint8 Col)
{
	return Board->GetCardDataRowName(Line, Col);
}

void ANAFGameMode::CopyCardInBoard(uint8 IndexHandCard, uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col, FCardDataServer Card)
{
	IndexHandCardPlayed = IndexHandCard;
	Board->CopyCard(Card1Line, Card1Col, Card2Line, Card2Col, Card);
}

FName ANAFGameMode::GetRowNameFromDataServer(FCardDataServer Card)
{
	return Deck->GetRowNameFromDataServer(Card);
}

void ANAFGameMode::SetGameOverInfos(EPosition WinnerId)
{
	bIsGameOver = true;
	Winner = WinnerId;
}


void ANAFGameMode::GiveOneCardToBothPlayer()
{
	// Give one card to both player
	float GiveCardDelay = 1.f;
	for (APlayerState* PlayerState : NafGameState->PlayerArray)
	{
		if (PlayerState)
		{
			if (ANAFPlayerState* NafPS = Cast<ANAFPlayerState>(PlayerState))
			{
				FCardDataServer Card = Deck->DrawCard();
				NafPS->StoreCardInHand(Card);
				TArray<bool> HandCurrent = NafPS->HandStatus();
				ANAFPlayerState* OpponentPS = NafGameState->GetOpponentPlayerState(NafPS->Id);
				OpponentPS->UpdateHandUI(NafPS->Id,HandCurrent);
			}
		}
	}
}

void ANAFGameMode::InitializeCurrentPlayer()
{
	// Initialize Active Player
	const bool bIsPlayerLeft = FMath::RandBool();
	const EPosition ActivePosition = bIsPlayerLeft ? EPosition::LEFT : EPosition::RIGHT;
	
	NafGameState->SetActivePlayer(ActivePosition);
}





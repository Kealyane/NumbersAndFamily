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
	
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString(TEXT("game mode : post login")));
	// }
	UE_LOG(LogTemp, Warning, TEXT("game mode : post login"));

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
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString(TEXT("game mode : launch game")));
		UE_LOG(LogTemp, Warning, TEXT("game mode : LaunchGame"));

		UWorld* World = GetWorld();
		
		if (World)
		{
			World->GetTimerManager().ClearTimer(WaitHandle);
		}

		// Link to Level Blueprint to remove lobby widget
		NafGameState = GetGameState<ANAFGameState>();
		
		if (NafGameState)
		{
			NafGameState->SetStatus(EGameStatus::START);
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
			Deck->InitDeck();
			Board = World->SpawnActor<ABoard>(BoardClassType);
			Board->InitBoard();
			NafGameState->InitBoardRow();
		}

		NafGameState->MultiRPC_PlaySoundStartGame();
		
		for (APlayerState* PlayerState : NafGameState->PlayerArray)
		{
			if (PlayerState)
			{
				if (ANAFPlayerState* NafPS = Cast<ANAFPlayerState>(PlayerState))
				{
					const FCardDataServer Card = Deck->DrawCard();
					NafPS->StoreCardInHand(Card);
					TArray<bool> HandCurrent = NafPS->HandStatus();
					ANAFPlayerState* OpponentPS = NafGameState->GetOpponentPlayerState(NafPS->Id);
					OpponentPS->UpdateHandUI(NafPS->Id,HandCurrent);
				}
			}
		}

		// Initialize Active Player
		const bool bIsPlayerLeft = FMath::RandBool();
		//const EPosition ActivePosition = bIsPlayerLeft ? EPosition::LEFT : EPosition::RIGHT;

		// DEBUG Client
		const EPosition ActivePosition = EPosition::RIGHT;

		// DEBUG Server
		//const EPosition ActivePosition = EPosition::LEFT;
		
		NafGameState->SetActivePlayer(ActivePosition);
		NafGameState->MultiRPC_UpdateActiveTurnUI();
		NafGameState->SetStatus(EGameStatus::IN_GAME);
	}
}

void ANAFGameMode::DrawCard(ANAFPlayerState* ActivePlayerState)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString(TEXT("game mode : DrawCard")));
	UE_LOG(LogTemp, Warning, TEXT("game mode : DrawCard"));
	const FCardDataServer Card = Deck->DrawCard();
	ActivePlayerState->StoreCardInHand(Card);
	ActivePlayerState->ActiveHandChoice(ActivePlayerState->Id);
	TArray<bool> HandCurrent = ActivePlayerState->HandStatus();
	ANAFPlayerState* OpponentPS = NafGameState->GetOpponentPlayerState(ActivePlayerState->Id);
	OpponentPS->UpdateHandUI(ActivePlayerState->Id,HandCurrent);
}

void ANAFGameMode::RemoveCardFromHand(ANAFPlayerState* ActivePlayerState)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString(TEXT("game mode : RemoveCardFromHand")));
	UE_LOG(LogTemp, Warning, TEXT("game mode : RemoveCardFromHand"));
	ActivePlayerState->RemoveCardInHand(IndexHandCardPlayed);
	TArray<bool> HandCurrent = ActivePlayerState->HandStatus();
	ANAFPlayerState* OpponentPS = NafGameState->GetOpponentPlayerState(ActivePlayerState->Id);
	OpponentPS->UpdateHandUI(ActivePlayerState->Id,HandCurrent);
}

void ANAFGameMode::PlaceNormalCard(FCardDataServer Card, uint8 IndexHandCard, uint8 Line, uint8 Col)
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString(TEXT("game mode : PlaceNormalCard")));
	UE_LOG(LogTemp, Warning, TEXT("game mode : PlaceNormalCard"));
	IndexHandCardPlayed = IndexHandCard;
	Board->PlaceNormalCard(Card, Line, Col);
}

void ANAFGameMode::EndTurn()
{
	//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString(TEXT("game mode : EndTurn")));
	UE_LOG(LogTemp, Warning, TEXT("game mode : EndTurn"));
	NafGameState->SwitchPlayerTurn();
}



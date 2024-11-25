// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NAFGameMode.h"
#include "GameFramework/GameState.h"
#include "GameMode/NAFGameState.h"
#include "Player/NAFPlayerController.h"
#include "Player/NAFPlayerState.h"
#include "GameElements/Deck.h"

void ANAFGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString(TEXT("game mode : post login")));
	}

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
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString(TEXT("game mode : launch game")));

		UWorld* World = GetWorld();
		
		if (World)
		{
			World->GetTimerManager().ClearTimer(WaitHandle);
		}

		// Link to Level Blueprint to remove lobby widget
		NafGameState = GetGameState<ANAFGameState>();
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
			Deck = World->SpawnActor<ADeck>(DeckType);
			Deck->InitDeck();
		}

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
		const EPosition ActivePosition = bIsPlayerLeft ? EPosition::LEFT : EPosition::RIGHT;
		NafGameState->SetActivePlayer(ActivePosition);
		ActivePlayerTurn(ActivePosition);
	}
	
}

void ANAFGameMode::ActivePlayerTurn(EPosition Id)
{
	if(ANAFPlayerState* ActivePS = NafGameState->GetNafPlayerState(Id))
	{
		ActivePS->ActiveTurn(Id);
	}
	NafGameState->UpdateActiveTurnUI();
}

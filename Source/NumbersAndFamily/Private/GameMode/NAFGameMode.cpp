// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NAFGameMode.h"
#include "GameFramework/GameState.h"
#include "GameMode/NAFGameState.h"
#include "Player/NAFPlayerController.h"
#include "Player/NAFPlayerState.h"

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

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(WaitHandle);
		}

		// Link to Level Blueprint to remove lobby widget
		if (ANAFGameState* NafGameState = GetGameState<ANAFGameState>())
		{
			NafGameState->OnLeaveLobby.Broadcast();
		}

		for (FConstPlayerControllerIterator PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; ++PCIterator)
		{
			if (ANAFPlayerController* NafPlayerController = Cast<ANAFPlayerController>(*PCIterator))
			{
				NafPlayerController->ClientRPC_ShowGameBoard();
			}
		}
	}
	
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NAFGameMode.h"
#include "GameFramework/GameState.h"

void ANAFGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString(TEXT("game mode : post login")));
	}
	
	int32 NumberOfPlayers = GameState->PlayerArray.Num();

	if (NumberOfPlayers == 2)
	{
		OnLeaveLobby.Broadcast();
	}
}

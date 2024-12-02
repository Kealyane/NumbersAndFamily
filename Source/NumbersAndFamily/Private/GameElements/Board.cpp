// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/Board.h"
#include "GameMode/NAFGameState.h"

ABoard::ABoard()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ABoard::InitBoard()
{
	BoardGame.SetNum(NB_LINE);
	BoardGame[0].Init(FCardDataServer(),NB_COLUMN);
	BoardGame[1].Init(FCardDataServer(),NB_COLUMN);
	BoardGame[2].Init(FCardDataServer(),NB_COLUMN);
	
	BoardRowNames.Init(FName("NONE"),NB_LINE * NB_COLUMN);
}

void ABoard::PlaceNormalCard(FCardDataServer Card, uint8 Line, uint8 Col)
{
	UE_LOG(LogTemp, Warning, TEXT("Board Place normal card"));
	if (!BoardGame[Line][Col].RowName.IsNone()) return;
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString(TEXT("board : PlaceNormalCard")));
	BoardGame[Line][Col] = Card;
	SyncBoardWithGameState();
}

void ABoard::SyncBoardWithGameState()
{
	UE_LOG(LogTemp, Warning, TEXT("Board SyncBoardWithGameState"));
	int index = 0;
	for (int i = 0; i < NB_LINE; i++)
	{
		for (int j = 0; j < NB_COLUMN; j++)
		{
			// if (BoardGame[i][j].RowName.IsNone())
			// {
			// 	BoardRowNames[index] = FName("NONE");
			// }
			// else
			// {
				BoardRowNames[index] = BoardGame[i][j].RowName;
			//}
			index++;
		}
	}
	if (ANAFGameState* NafGS = GetWorld()->GetGameState<ANAFGameState>())
	{
		NafGS->SetBoardName(BoardRowNames);
	}
	
}


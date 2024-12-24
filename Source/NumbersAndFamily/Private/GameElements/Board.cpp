// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/Board.h"

#include "GameMode/NAFGameMode.h"
#include "GameMode/NAFGameState.h"

class ANAFGameMode;

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

void ABoard::InitParamDeck(ADeck* InDeck)
{
	Deck = InDeck;
}

void ABoard::PlaceNormalCard(FCardDataServer Card, uint8 Line, uint8 Col)
{
	UE_LOG(LogTemp, Warning, TEXT("Board Place normal card"));
	if (!BoardGame[Line][Col].RowName.IsNone()) return;
	GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString(TEXT("board : PlaceNormalCard")));
	BoardGame[Line][Col] = Card;
	
	DeleteCardWithSameScore(Line, Col);
	if (Col == 0 || Col == 5) DeleteCardsBecauseOfFamily(Line, Col);
	
	SyncBoardWithGameState();
}

bool ABoard::IsCoordOccupied(uint8 Line, uint8 Col)
{
	return !BoardGame[Line][Col].RowName.IsNone();
}

void ABoard::SwitchCard(uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col)
{
	FCardDataServer Tmp = BoardGame[Card1Line][Card1Col];
	BoardGame[Card1Line][Card1Col] = BoardGame[Card2Line][Card2Col];
	BoardGame[Card2Line][Card2Col] = Tmp;
	
	DeleteCardWithSameScore(Card1Line, Card1Col);
	if (Card1Col == 0 || Card1Col == 5) DeleteCardsBecauseOfFamily(Card1Line, Card1Col);
	
	DeleteCardWithSameScore(Card2Line, Card2Col);
	if (Card2Col == 0 || Card2Col == 5) DeleteCardsBecauseOfFamily(Card2Line, Card2Col);
	
	SyncBoardWithGameState();
}

void ABoard::StealCard(uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col)
{
	BoardGame[Card2Line][Card2Col] = BoardGame[Card1Line][Card1Col];
	BoardGame[Card1Line][Card1Col].ResetCard();
	
	DeleteCardWithSameScore(Card2Line, Card2Col);
	if (Card2Col == 0 || Card2Col == 5) DeleteCardsBecauseOfFamily(Card2Line, Card2Col);
	
	SyncBoardWithGameState();
}

FName ABoard::GetCardDataRowName(uint8 Line, uint8 Col)
{
	if (Line >= 3 && Col >= 6) return FName("NONE");
	return BoardGame[Line][Col].RowName;
}

void ABoard::CopyCard(uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col, FCardDataServer Card)
{
	UE_LOG(LogTemp, Warning, TEXT("Board CopyCard"));
	//Card.DebugCard("Board Copy Card : ");
	Card.SetArcaneFromCopy(BoardGame[Card1Line][Card1Col]);
	//Card.DebugCard("Board Copy Card after set : ");
	BoardGame[Card2Line][Card2Col] = Card;

	DeleteCardWithSameScore(Card2Line, Card2Col);
	if (Card2Col == 0 || Card2Col == 5) DeleteCardsBecauseOfFamily(Card2Line, Card2Col);
	
	SyncBoardWithGameState();
}

void ABoard::DeleteCardWithSameScore(uint8 Line, uint8 Col)
{
	const int32 CardScore = BoardGame[Line][Col].Score;
	if (Col < 3)
	{
		// Look in Player 2 side
		for (int i = 3; i < 6; i++)
		{
			if (!BoardGame[Line][i].RowName.IsNone())
			{
				if (BoardGame[Line][i].Score == CardScore)
				{
					if (BoardGame[Line][i].ArcaneType != EArcaneType::COPY)
					{
						//BoardGame[Line][i].DebugCard("DeleteCardWithSameScore");
						Deck->BackToDeck(BoardGame[Line][i]);
					}
					BoardGame[Line][i].ResetCard();
				}
			}
		}
	}
	else
	{
		// Look in Player 1 side
		for (int i = 0; i < 3; i++)
		{
			if (!BoardGame[Line][i].RowName.IsNone())
			{
				if (BoardGame[Line][i].Score == CardScore)
				{
					if (BoardGame[Line][i].ArcaneType != EArcaneType::COPY)
					{
						//BoardGame[Line][i].DebugCard("DeleteCardWithSameScore");
						Deck->BackToDeck(BoardGame[Line][i]);
					}
					BoardGame[Line][i].ResetCard();
				}
			}
		}
	}
}

void ABoard::DeleteCardsBecauseOfFamily(uint8 Line, uint8 Col)
{
	EFamilyType CardFamily = BoardGame[Line][Col].FamilyType;
	if (Col == 0)
	{
		// Look in Player 1 side
		bool bSameFamily = CardFamily == BoardGame[Line][1].FamilyType && CardFamily == BoardGame[Line][2].FamilyType;

		if (bSameFamily)
		{
			// Remove line in Player 2 side
			for (int i = 3; i < 6; i++)
			{
				if (BoardGame[Line][i].RowName.IsNone()) continue;
				
				if (BoardGame[Line][i].ArcaneType != EArcaneType::COPY)
				{
					//BoardGame[Line][i].DebugCard("DeleteCardsBecauseOfFamily");
					Deck->BackToDeck(BoardGame[Line][i]);
				}
				BoardGame[Line][i].ResetCard();
			}			
		}
	}
	else
	{
		// Look in Player 2 side
		bool bSameFamily = CardFamily == BoardGame[Line][3].FamilyType && CardFamily == BoardGame[Line][4].FamilyType;

		if (bSameFamily)
		{
			// Remove line in Player 1 side
			for (int i = 0; i < 3; i++)
			{
				if (BoardGame[Line][i].RowName.IsNone()) continue;
				
				if (BoardGame[Line][i].ArcaneType != EArcaneType::COPY)
				{
					//BoardGame[Line][i].DebugCard("DeleteCardsBecauseOfFamily");
					Deck->BackToDeck(BoardGame[Line][i]);
				}
				BoardGame[Line][i].ResetCard();
			}			
		}
	}
}


void ABoard::SyncBoardWithGameState()
{
	//UE_LOG(LogTemp, Warning, TEXT("Board SyncBoardWithGameState"));
	int index = 0;
	
	for (int i = 0; i < NB_LINE; i++)
	{
		for (int j = 0; j < NB_COLUMN; j++)
		{
			if (BoardGame[i][j].ArcaneType == EArcaneType::COPY)
			{
				if (ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode()))
				{
					FName CopyCardName = GameMode->GetRowNameFromDataServer(BoardGame[i][j]);
					BoardRowNames[index] = CopyCardName;
					UE_LOG(LogTemp, Warning, TEXT("Board SyncBoardWithGameState : card to copy %s"),*CopyCardName.ToString());
				}
			}
			else
			{
				BoardRowNames[index] = BoardGame[i][j].RowName;
			}
			index++;
		}
	}
	if (ANAFGameState* NafGS = GetWorld()->GetGameState<ANAFGameState>())
	{
		NafGS->SetBoardName(true, BoardRowNames);
	}
	
}


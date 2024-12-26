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
	if (!BoardGame[Line][Col].RowName.IsNone()) return;
	BoardGame[Line][Col] = Card;
	
	if (IsLineFull(Line, Col) && IsFamily(Line, Col)) DeleteCardsBecauseOfFamily(Line, Col);
	else DeleteCardWithSameScore(Line, Col);
	
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
	
	if (IsLineFull(Card1Line,Card1Col) && IsFamily(Card1Line, Card1Col)) DeleteCardsBecauseOfFamily(Card1Line, Card1Col);
	else DeleteCardWithSameScore(Card1Line, Card1Col);
	
	if (IsLineFull(Card2Line, Card2Col) && IsFamily(Card2Line, Card2Col)) DeleteCardsBecauseOfFamily(Card2Line, Card2Col);
	else DeleteCardWithSameScore(Card2Line, Card2Col);
	
	SyncBoardWithGameState();
}

void ABoard::StealCard(uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col)
{
	BoardGame[Card2Line][Card2Col] = BoardGame[Card1Line][Card1Col];
	BoardGame[Card1Line][Card1Col].ResetCard();

	if (IsLineFull(Card2Line, Card2Col) && IsFamily(Card2Line, Card2Col)) DeleteCardsBecauseOfFamily(Card2Line, Card2Col);
	else DeleteCardWithSameScore(Card2Line, Card2Col);
	
	if (HasHoles(Card1Line, Card1Col)) MoveCards(Card1Line, Card1Col);
	
	SyncBoardWithGameState();
}

FName ABoard::GetCardDataRowName(uint8 Line, uint8 Col)
{
	if (Line >= 3 && Col >= 6) return FName("NONE");
	return BoardGame[Line][Col].RowName;
}

void ABoard::CopyCard(uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col, FCardDataServer Card)
{
	Card.SetArcaneFromCopy(BoardGame[Card1Line][Card1Col]);
	BoardGame[Card2Line][Card2Col] = Card;

	if (IsLineFull(Card2Line, Card2Col) && IsFamily(Card2Line, Card2Col)) DeleteCardsBecauseOfFamily(Card2Line, Card2Col);
	else DeleteCardWithSameScore(Card2Line, Card2Col);
	
	SyncBoardWithGameState();
}

void ABoard::DeleteCardWithSameScore(uint8 Line, uint8 Col)
{
	const int32 CardScore = BoardGame[Line][Col].Score;
	uint8 Counter = 0;
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
						Deck->BackToDeck(BoardGame[Line][i]);
					}
					BoardGame[Line][i].ResetCard();
					Counter++;
				}
				else
				{
					BoardGame[Line][i-Counter] = BoardGame[Line][i];
					if (Counter != 0) BoardGame[Line][i].ResetCard();
				}
			}
		}
	}
	else
	{
		// Look in Player 1 side
		for (int i = 2; i >= 0; i--)
		{
			if (!BoardGame[Line][i].RowName.IsNone())
			{
				if (BoardGame[Line][i].Score == CardScore)
				{
					if (BoardGame[Line][i].ArcaneType != EArcaneType::COPY)
					{
						Deck->BackToDeck(BoardGame[Line][i]);
					}
					BoardGame[Line][i].ResetCard();
					Counter++;
				}
				else
				{
					BoardGame[Line][i+Counter] = BoardGame[Line][i];
					if (Counter != 0) BoardGame[Line][i].ResetCard();
				}
			}
		}
	}
}

void ABoard::DeleteCardsBecauseOfFamily(uint8 Line, uint8 Col)
{
	if (Col < 3)
	{
		// Remove line in Player 2 side
		for (int i = 3; i < 6; i++)
		{
			if (BoardGame[Line][i].RowName.IsNone()) continue;
			
			if (BoardGame[Line][i].ArcaneType != EArcaneType::COPY)
			{
				Deck->BackToDeck(BoardGame[Line][i]);
			}
			BoardGame[Line][i].ResetCard();
		}
	}
	else
	{
		// Remove line in Player 1 side
		for (int i = 0; i < 3; i++)
		{
			if (BoardGame[Line][i].RowName.IsNone()) continue;
			
			if (BoardGame[Line][i].ArcaneType != EArcaneType::COPY)
			{
				Deck->BackToDeck(BoardGame[Line][i]);
			}
			BoardGame[Line][i].ResetCard();
		}
	}
}


void ABoard::SyncBoardWithGameState()
{
	ComputeScores();
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
					//UE_LOG(LogTemp, Warning, TEXT("Board SyncBoardWithGameState : card to copy %s"),*CopyCardName.ToString());
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

bool ABoard::IsLineFull(uint8 Line, uint8 Col)
{
	if (Col < 3)
	{
		for (int i = 0; i < 3; i++)
		{
			if (BoardGame[Line][i].RowName.IsNone()) return false;
		}
		return true;
	}
	for (int i = 3; i < 6; i++)
	{
		if (BoardGame[Line][i].RowName.IsNone()) return false;
	}
	return true;
}

bool ABoard::IsFamily(uint8 Line, uint8 Col)
{
	if (Col < 3)
	{
		return BoardGame[Line][0].FamilyType == BoardGame[Line][1].FamilyType &&
			BoardGame[Line][0].FamilyType == BoardGame[Line][2].FamilyType;
	}
	return BoardGame[Line][5].FamilyType == BoardGame[Line][3].FamilyType &&
		BoardGame[Line][5].FamilyType == BoardGame[Line][4].FamilyType;
}

void ABoard::MoveCards(uint8 Line, uint8 Col)
{
	TArray<FCardDataServer> NewLine;
	NewLine.Init(FCardDataServer(),3);
	
	if (Col < 3)
	{
		for (int i = 2; i >= 0; i--)
		{
			if (BoardGame[Line][i].RowName.IsNone()) continue;
			NewLine[i] = BoardGame[Line][i];
		}
		NewLine.Append(&BoardGame[Line][3],3);
	}
	else
	{
		NewLine.Append(&BoardGame[Line][0],3);
		for (int i = 3; i < 6; i++)
		{
			if (BoardGame[Line][i].RowName.IsNone()) continue;
			NewLine[i] = BoardGame[Line][i];
		}
	}
	BoardGame[Line] = NewLine;
}

bool ABoard::HasHoles(uint8 Line, uint8 Col)
{
	if (Col < 3)
	{
		return BoardGame[Line][2].RowName.IsNone() && (!BoardGame[Line][1].RowName.IsNone() || !BoardGame[Line][0].RowName.IsNone()) ||
			BoardGame[Line][1].RowName.IsNone() && !BoardGame[Line][0].RowName.IsNone();
	}
	return BoardGame[Line][3].RowName.IsNone() && (!BoardGame[Line][4].RowName.IsNone() || !BoardGame[Line][5].RowName.IsNone()) ||
			BoardGame[Line][4].RowName.IsNone() && !BoardGame[Line][5].RowName.IsNone();
}

void ABoard::ComputeScores()
{
	int32 LeftScore0 = ComputeLineScoreLeft(0);
	int32 LeftScore1 = ComputeLineScoreLeft(1);
	int32 LeftScore2 = ComputeLineScoreLeft(2);
	int32 LeftTotalScore = LeftScore0 + LeftScore1 + LeftScore2;
	int32 RightScore0 = ComputeLineScoreRight(0);
	int32 RightScore1 = ComputeLineScoreRight(1);
	int32 RightScore2 = ComputeLineScoreRight(2);
	int32 RightTotalScore = RightScore0 + RightScore1 + RightScore2;

	if (ANAFGameState* GameState = GetWorld()->GetGameState<ANAFGameState>())
	{
		// Update UI
		GameState->MultiRPC_UpdateScores(LeftScore0, LeftScore1, LeftScore2, LeftTotalScore,
								RightScore0, RightScore1, RightScore2, RightTotalScore);
		// Update Datas (Server/Client)
		GameState->UpdateScores(LeftScore0, LeftScore1, LeftScore2, LeftTotalScore,
								RightScore0, RightScore1, RightScore2, RightTotalScore);
	}
}

int32 ABoard::ComputeLineScoreLeft(uint8 Line)
{
	if (BoardGame[Line][0].Score == BoardGame[Line][1].Score && BoardGame[Line][1].Score == BoardGame[Line][2].Score)
	{
		return BoardGame[Line][0].Score * 3;
	}
	if (BoardGame[Line][0].Score == BoardGame[Line][1].Score)
	{
		return BoardGame[Line][0].Score * 2 + BoardGame[Line][2].Score;
	}
	if (BoardGame[Line][1].Score == BoardGame[Line][2].Score)
	{
		return BoardGame[Line][1].Score * 2 + BoardGame[Line][0].Score;
	}
	if (BoardGame[Line][0].Score == BoardGame[Line][2].Score)
	{
		return BoardGame[Line][0].Score * 2 + BoardGame[Line][1].Score;
	}
	return BoardGame[Line][0].Score + BoardGame[Line][1].Score + BoardGame[Line][2].Score;
}

int32 ABoard::ComputeLineScoreRight(uint8 Line)
{
	if (BoardGame[Line][3].Score == BoardGame[Line][4].Score && BoardGame[Line][4].Score == BoardGame[Line][5].Score)
	{
		return BoardGame[Line][3].Score * 3;
	}
	if (BoardGame[Line][3].Score == BoardGame[Line][4].Score)
	{
		return BoardGame[Line][3].Score * 2 + BoardGame[Line][5].Score;
	}
	if (BoardGame[Line][4].Score == BoardGame[Line][5].Score)
	{
		return BoardGame[Line][4].Score * 2 + BoardGame[Line][3].Score;
	}
	if (BoardGame[Line][3].Score == BoardGame[Line][5].Score)
	{
		return BoardGame[Line][3].Score * 2 + BoardGame[Line][4].Score;
	}
	return BoardGame[Line][3].Score + BoardGame[Line][4].Score + BoardGame[Line][5].Score;
}


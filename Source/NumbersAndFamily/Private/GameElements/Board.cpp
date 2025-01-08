// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/Board.h"

#include "GameMode/NAFGameMode.h"
#include "GameMode/NAFGameState.h"
#include "NumbersAndFamily/NumbersAndFamily.h"

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

	bCardDestruction = false;
}

void ABoard::ResetBoard()
{
	BoardGame.Empty();
	BoardRowNames.Empty();
	bCardDestruction = false;
	InitBoard();
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
	
	FTimerHandle SynHandle;
	GetWorld()->GetTimerManager().SetTimer(SynHandle, [this, Line, Col]()
	{
		SyncBoardWithGameState();
	},
	bCardDestruction ? 1.4f : 0.1f, false);
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

	FTimerHandle SynHandle;
	GetWorld()->GetTimerManager().SetTimer(SynHandle, [this]()
	{
		SyncBoardWithGameState();
		if (ANAFGameState* NafGS = GetWorld()->GetGameState<ANAFGameState>())
		{
			NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::Switch);
		}
	},
	bCardDestruction ? 1.4f : 0.1f, false);
	
}

void ABoard::StealCard(uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col)
{
	BoardGame[Card2Line][Card2Col] = BoardGame[Card1Line][Card1Col];
	BoardGame[Card1Line][Card1Col].ResetCard();

	if (IsLineFull(Card2Line, Card2Col) && IsFamily(Card2Line, Card2Col)) DeleteCardsBecauseOfFamily(Card2Line, Card2Col);
	else DeleteCardWithSameScore(Card2Line, Card2Col);
	
	if (HasHoles(Card1Line, Card1Col)) MoveCards(Card1Line, Card1Col);
	
	FTimerHandle SynHandle;
	GetWorld()->GetTimerManager().SetTimer(SynHandle, [this]()
	{
		SyncBoardWithGameState();
		if (ANAFGameState* NafGS = GetWorld()->GetGameState<ANAFGameState>())
		{
			NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::Steal);
		}
	},
	bCardDestruction ? 1.4f : 0.1f, false);
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
	
	FTimerHandle SynHandle;
	GetWorld()->GetTimerManager().SetTimer(SynHandle, [this]()
	{
		SyncBoardWithGameState();
		if (ANAFGameState* NafGS = GetWorld()->GetGameState<ANAFGameState>())
		{
			NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::Steal);
		}
	},
	bCardDestruction ? 1.4f : 0.1f, false);
}

void ABoard::DeleteCardWithSameScore(uint8 Line, uint8 Col)
{
	const int32 CardScore = BoardGame[Line][Col].Score;
	TArray<FIntPoint> CoordCardsDeleted;
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
					CoordCardsDeleted.Add(FIntPoint(Line, i));
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
					CoordCardsDeleted.Add(FIntPoint(Line, i));
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
	if (ANAFGameState* NafGS = GetWorld()->GetGameState<ANAFGameState>())
	{
		if (CoordCardsDeleted.Num() > 0)
		{
			bCardDestruction = true;
			NafGS->MultiRPC_NumEffect(CoordCardsDeleted);
			NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::DestructionCard);
		}
	}
}

void ABoard::DeleteCardsBecauseOfFamily(uint8 Line, uint8 Col)
{
	ANAFGameState* NafGS = GetWorld()->GetGameState<ANAFGameState>();
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
			if (NafGS)
			{
				NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::DestructionCard);
				NafGS->MultiRPC_FamilyEffect(2, Line);
			}
			bCardDestruction = true;
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
			if (NafGS)
			{
				NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::DestructionCard);
				NafGS->MultiRPC_FamilyEffect(1, Line);
			}
			bCardDestruction = true;
		}
	}
}


void ABoard::SyncBoardWithGameState()
{

	GetWorld()->GetTimerManager().SetTimer(ComputeScoreHandle, this, &ABoard::ComputeScores, 1.f, false);

	bCardDestruction = false;

	uint8 NbCardsLeft = 0;
	uint8 NbCardsRight = 0;
	
	int index = 0;
	
	ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode());
	
	for (int i = 0; i < NB_LINE; i++)
	{
		for (int j = 0; j < NB_COLUMN; j++)
		{
			if (!BoardGame[i][j].RowName.IsNone()) j < 3 ? NbCardsLeft++ : NbCardsRight++;

			if (BoardGame[i][j].ArcaneType == EArcaneType::COPY)
			{
				if (GameMode)
				{
					FName CopyCardName = GameMode->GetRowNameFromDataServer(BoardGame[i][j]);
					BoardRowNames[index] = CopyCardName;
				}
			}
			else
			{
				BoardRowNames[index] = BoardGame[i][j].RowName;
			}
			index++;
		}
	}
	
	// Check GameOver
	if (NbCardsLeft == 9) GameMode->SetGameOverInfos(EPosition::LEFT);
	if (NbCardsRight == 9)GameMode->SetGameOverInfos(EPosition::RIGHT);

	// Set the board
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

	if (ANAFGameState* NafGS = GetWorld()->GetGameState<ANAFGameState>())
	{
		NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::MoveCardAppear);
	}
	
	if (Col < 3)
	{
		int j = 2;
		NewLine.Init(FCardDataServer(),3);
		for (int i = 2; i >= 0; i--)
		{
			if (BoardGame[Line][i].RowName.IsNone()) continue;
			NewLine[j] = BoardGame[Line][i];
			j--;
		}
		NewLine.Append(&BoardGame[Line][3],3);
	}
	else
	{
		NewLine.Append(&BoardGame[Line][0],3);
		for (int i = 3; i < 6; i++)
		{
			if (BoardGame[Line][i].RowName.IsNone()) continue;
			NewLine.Add(BoardGame[Line][i]);
		}
		if (NewLine.Num() != 6)
		{
			for (int i = NewLine.Num(); i < 6; i++)
			{
				NewLine.Add(FCardDataServer());
			}
		}	
	}
	BoardGame[Line] = NewLine;
}

bool ABoard::HasHoles(uint8 Line, uint8 Col)
{
	if (Col < 3)
	{
		return BoardGame[Line][2].RowName.IsNone() && (!BoardGame[Line][1].RowName.IsNone() || !BoardGame[Line][0].RowName.IsNone()) ||
			BoardGame[Line][1].RowName.IsNone() && (!BoardGame[Line][0].RowName.IsNone() || !BoardGame[Line][2].RowName.IsNone());
	}
	return BoardGame[Line][3].RowName.IsNone() && (!BoardGame[Line][4].RowName.IsNone() || !BoardGame[Line][5].RowName.IsNone()) ||
			BoardGame[Line][4].RowName.IsNone() && (!BoardGame[Line][5].RowName.IsNone() || !BoardGame[Line][3].RowName.IsNone());
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
		GameState->SetScoresServer(LeftScore0, LeftScore1, LeftScore2,
						RightScore0, RightScore1, RightScore2);
	}
}

int32 ABoard::ComputeLineScoreLeft(uint8 Line)
{
	TArray<FIntPoint> CoordCardsMult;
	int total = 0;
	
	ANAFGameState* NafGS = GetWorld()->GetGameState<ANAFGameState>();
	if (NafGS == nullptr) return total;
	
	int PreviousScore = 0;
	if (Line == 0) PreviousScore = NafGS->P1Score0;
	if (Line == 1) PreviousScore = NafGS->P1Score1;
	if (Line == 2) PreviousScore = NafGS->P1Score2;
	
	if (BoardGame[Line][0].Score != 0 && BoardGame[Line][0].Score == BoardGame[Line][1].Score && BoardGame[Line][1].Score == BoardGame[Line][2].Score)
	{
		total = (BoardGame[Line][0].Score * 3) * 3;
		
		if (total != PreviousScore)
		{
			CoordCardsMult.Add(FIntPoint(Line,0));
			CoordCardsMult.Add(FIntPoint(Line,1));
			CoordCardsMult.Add(FIntPoint(Line,2));
			NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::CounterMult3);
			NafGS->MultiRPC_Combo3(CoordCardsMult);
		}
		return total;
	}
	if (BoardGame[Line][0].Score != 0 && BoardGame[Line][0].Score == BoardGame[Line][1].Score)
	{
		total = (BoardGame[Line][0].Score * 2) * 2 + BoardGame[Line][2].Score;
		if (total != PreviousScore)
		{
			CoordCardsMult.Add(FIntPoint(Line,0));
			CoordCardsMult.Add(FIntPoint(Line,1));
			NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::CounterMult2);
			NafGS->MultiRPC_Combo2(CoordCardsMult);
		}
		return total;
	}
	if (BoardGame[Line][1].Score != 0 && BoardGame[Line][1].Score == BoardGame[Line][2].Score)
	{
		total = (BoardGame[Line][1].Score * 2) * 2 + BoardGame[Line][0].Score;
		if (total != PreviousScore)
		{
			CoordCardsMult.Add(FIntPoint(Line,1));
			CoordCardsMult.Add(FIntPoint(Line,2));
			NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::CounterMult2);
			NafGS->MultiRPC_Combo2(CoordCardsMult);
		}
		return total;
	}
	if (BoardGame[Line][0].Score != 0 && BoardGame[Line][0].Score == BoardGame[Line][2].Score)
	{
		total = (BoardGame[Line][0].Score * 2) * 2 + BoardGame[Line][1].Score;
		if (total != PreviousScore)
		{
			CoordCardsMult.Add(FIntPoint(Line,0));
			CoordCardsMult.Add(FIntPoint(Line,2));
			NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::CounterMult2);
			NafGS->MultiRPC_Combo2(CoordCardsMult);
		}
		return total;
	}

	NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::CounterUp);
	return BoardGame[Line][0].Score + BoardGame[Line][1].Score + BoardGame[Line][2].Score;
}

int32 ABoard::ComputeLineScoreRight(uint8 Line)
{
	TArray<FIntPoint> CoordCardsMult;
	int total = 0;
	
	ANAFGameState* NafGS = GetWorld()->GetGameState<ANAFGameState>();
	if (NafGS == nullptr) return total;
	
	int PreviousScore = 0;
	if (Line == 0) PreviousScore = NafGS->P2Score0;
	if (Line == 1) PreviousScore = NafGS->P2Score1;
	if (Line == 2) PreviousScore = NafGS->P2Score2;
	
	if (BoardGame[Line][3].Score != 0 && BoardGame[Line][3].Score == BoardGame[Line][4].Score && BoardGame[Line][4].Score == BoardGame[Line][5].Score)
	{
		total = (BoardGame[Line][3].Score * 3) * 3;
		if (total != PreviousScore)
		{
			CoordCardsMult.Add(FIntPoint(Line,3));
			CoordCardsMult.Add(FIntPoint(Line,4));
			CoordCardsMult.Add(FIntPoint(Line,5));
			NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::CounterMult3);
			NafGS->MultiRPC_Combo3(CoordCardsMult);
		}
		return total;
	}
	if (BoardGame[Line][3].Score != 0 && BoardGame[Line][3].Score == BoardGame[Line][4].Score)
	{
		total = (BoardGame[Line][3].Score * 2) * 2 + BoardGame[Line][5].Score;
		if (total != PreviousScore)
		{
			CoordCardsMult.Add(FIntPoint(Line,3));
			CoordCardsMult.Add(FIntPoint(Line,4));
			NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::CounterMult2);
			NafGS->MultiRPC_Combo2(CoordCardsMult);
		}
		return total;
	}
	if (BoardGame[Line][4].Score != 0 && BoardGame[Line][4].Score == BoardGame[Line][5].Score)
	{
		total = (BoardGame[Line][4].Score * 2) * 2 + BoardGame[Line][3].Score;
		if (total != PreviousScore)
		{
			CoordCardsMult.Add(FIntPoint(Line,4));
			CoordCardsMult.Add(FIntPoint(Line,5));
			NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::CounterMult2);
			NafGS->MultiRPC_Combo2(CoordCardsMult);
		}
		return total;
	}
	if (BoardGame[Line][3].Score != 0 && BoardGame[Line][3].Score == BoardGame[Line][5].Score)
	{
		total = (BoardGame[Line][3].Score * 2) * 2 + BoardGame[Line][4].Score;
		if (total != PreviousScore)
		{
			CoordCardsMult.Add(FIntPoint(Line,3));
			CoordCardsMult.Add(FIntPoint(Line,5));
			NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::CounterMult2);
			NafGS->MultiRPC_Combo2(CoordCardsMult);
		}
		return total;
	}

	NafGS->MultiRPC_PlaySoundForBoth(ESoundRow::CounterUp);
	return BoardGame[Line][3].Score + BoardGame[Line][4].Score + BoardGame[Line][5].Score;
}


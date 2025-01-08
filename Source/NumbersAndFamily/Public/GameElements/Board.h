// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameElements/Deck.h"
#include "Board.generated.h"


UCLASS()
class NUMBERSANDFAMILY_API ABoard : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoard();

	void InitBoard();
	void ResetBoard();
	void InitParamDeck(ADeck* InDeck);
	void PlaceNormalCard(FCardDataServer Card, uint8 Line, uint8 Col);
	bool IsCoordOccupied(uint8 Line, uint8 Col);
	void SwitchCard(uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col);
	void StealCard(uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col);
	FName GetCardDataRowName(uint8 Line, uint8 Col);
	void CopyCard(uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col, FCardDataServer Card);

protected:
	const int NB_LINE = 3;
	const int NB_COLUMN = 6;

private:
	TArray<TArray<FCardDataServer>> BoardGame;
	TArray<FName> BoardRowNames;
	TObjectPtr<ADeck> Deck;
	FTimerHandle ComputeScoreHandle;

	bool bCardDestruction;

	// Gameplay
	void DeleteCardWithSameScore(uint8 Line, uint8 Col);
	void DeleteCardsBecauseOfFamily(uint8 Line, uint8 Col);
	void MoveCards(uint8 Line, uint8 Col);

	// Sync
	void SyncBoardWithGameState();

	// Check
	bool IsLineFull(uint8 Line, uint8 Col);
	bool IsFamily(uint8 Line, uint8 Col);
	bool HasHoles(uint8 Line, uint8 Col);

	// Score
	void ComputeScores();
	int32 ComputeLineScoreLeft(uint8 Line);
	int32 ComputeLineScoreRight(uint8 Line);
};

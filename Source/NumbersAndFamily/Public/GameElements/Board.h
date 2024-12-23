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
	void PlaceNormalCard(FCardDataServer Card, uint8 Line, uint8 Col);
	bool IsCoordOccupied(uint8 Line, uint8 Col);
	void SwitchCard(uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col);

protected:
	const int NB_LINE = 3;
	const int NB_COLUMN = 6;

private:
	TArray<TArray<FCardDataServer>> BoardGame;
	TArray<FName> BoardRowNames;

	void SyncBoardWithGameState();
};

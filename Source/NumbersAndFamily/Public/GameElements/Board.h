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

protected:
	const int NB_LINE = 3;
	const int NB_COLUMN = 6;

private:
	TArray<TArray<FCardDataServer>> BoardGame;
	TArray<FName> BoardRowNames;

	void SyncBoardWithGameState();
};

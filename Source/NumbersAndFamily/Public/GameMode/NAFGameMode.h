// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NAFGameMode.generated.h"


struct FCardDataServer;
class ABoard;
class ANAFGameState;
enum class EPosition : uint8;
class ANAFPlayerState;
class ADeck;
/**
 * 
 */
UCLASS()
class NUMBERSANDFAMILY_API ANAFGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="GameElements")
	TSubclassOf<ADeck> DeckClassType;
	UPROPERTY(EditDefaultsOnly, Category="GameElements")
	TSubclassOf<ABoard> BoardClassType;
	
	void PostLogin(APlayerController* NewPlayer) override;
	void LaunchGame();
	void EndGame();

public:
	void DrawCard(ANAFPlayerState* ActivePlayerState);
	void EndTurn();

	void RemoveCardFromHand(ANAFPlayerState* ActivePlayerState);

	void PlaceNormalCard(FCardDataServer Card, uint8 IndexHandCard, uint8 Line, uint8 Col);
	void SwitchCardsInBoard(uint8 IndexHandCard, uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col);
	void StealCardInBoard(uint8 IndexHandCard, uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col);
	void CopyCardInBoard(uint8 IndexHandCard, uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col, FCardDataServer Card);
	
	bool IsCoordOccupiedInBoard(uint8 Line, uint8 Col);
	FName GetCardDataRowName(uint8 Line, uint8 Col);
	FName GetRowNameFromDataServer(FCardDataServer Card);
	void SetGameOverInfos(EPosition WinnerId);
	
private:
	FTimerHandle WaitHandle;
	TObjectPtr<ADeck> Deck;
	TObjectPtr<ABoard> Board;
	TObjectPtr<ANAFGameState> NafGameState;

	uint8 IndexHandCardPlayed;
	bool bIsGameOver;
	EPosition Winner; 
};

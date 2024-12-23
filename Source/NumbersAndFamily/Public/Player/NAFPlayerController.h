// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NumbersAndFamily/NumbersAndFamily.h"
#include "NAFPlayerController.generated.h"

struct FCardDataServer;
enum class ECardType : uint8;
class ANAFPlayerState;
class UGameWidget;
/**
 * 
 */
UCLASS()
class NUMBERSANDFAMILY_API ANAFPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	TObjectPtr<UGameWidget> GameWidget;
	bool bIsMyTurn;

protected:
	UPROPERTY(EditDefaultsOnly, Category="MyPropperties|Widget")
	TSubclassOf<UGameWidget> GameWidgetType;
	UPROPERTY(EditAnywhere, Category="MyPropperties|DataTable")
	TObjectPtr<UDataTable> DeckDataTable;
	UPROPERTY(EditAnywhere, Category="MyPropperties|Card")
	TObjectPtr<UTexture2D> CardVerso;

	void PlaySound(USoundBase* Sound);
	
public:
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowGameBoard();
	UFUNCTION(Client, Reliable)
	void ClientRPC_PlaceCardInPocketUI(EPosition PlayerPosition, uint8 Pos, FName CardRowName);
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowPocketCardVerso(EPosition PlayerPosition, uint8 Pos);
	UFUNCTION(Client, Reliable)
	void ClientRPC_PocketCardEmpty(EPosition PlayerPosition, uint8 Pos);
	UFUNCTION(Client, Reliable)
	void ClientRPC_ActiveHand(EPosition PlayerPosition);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_DrawCard();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_PlaceNormalCard(FCardDataServer Card, uint8 IndexHandCard, uint8 Line, uint8 Col);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_ActiveSwitch(EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col,
					EPosition Card2Pos, uint8 Card2Line, uint8 Card2Col);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_ActiveSteal(EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col,
					EPosition Card2Pos, uint8 Card2Line, uint8 Card2Col);
	UFUNCTION(Server, Reliable)
	void ServerRPC_EndTurn(ANAFPlayerState* ActivePlayerState);

	UFUNCTION()
	void UpdateActiveTurnUI(EPosition ActivePosition);
	UFUNCTION()
	void NotifyTurnStart(bool bInIsMyTurn);
	UFUNCTION()
	void EndTurn();
	UFUNCTION()
	void UpdateBoardCard(bool bAfterPlayerAction, const TArray<FName>& InBoardTableRow);


	UFUNCTION()
	void EnableCardSelectionUI(EPosition PlayerId, ECardType CardType);
	// UFUNCTION()
	// void DisableCardSelectionUI();

	UFUNCTION()
	void GetCardTypeSelected(uint8 PosInHand);
	UFUNCTION()
	void GetSelectedHandCard(uint8 Line, uint8 Col);
	UFUNCTION()
	void HandleSwitch(EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col,
					EPosition Card2Pos, uint8 Card2Line, uint8 Card2Col);
	UFUNCTION()
	void HandleSteal(EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col,
					EPosition Card2Pos, uint8 Card2Line, uint8 Card2Col); // Card2 is empty slot

	UFUNCTION()
	uint8 GetNbCardInBoard() const { return NbCardInBoard; }
	UFUNCTION()
	uint8 GetNbCardInOpponentBoard() const { return NBCardInOpponentBoard; }
	
private:
	bool IsCoordInPlayerIdSide(EPosition PlayerId, uint8 Line, uint8 Col);
	bool IsActivePlayer() const;
	EPosition GetPlayerId() const;
	uint8 NbCardInBoard = 0;
	uint8 NBCardInOpponentBoard = 0;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NumbersAndFamily/NumbersAndFamily.h"
#include "NAFPlayerController.generated.h"

class AAudioContainer;
class UEndGameWidget;
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
	TObjectPtr<UEndGameWidget> EndGameWidget;
	bool bIsMyTurn;

protected:
	UPROPERTY(EditDefaultsOnly, Category="MyPropperties|Widget")
	TSubclassOf<UGameWidget> GameWidgetType;
	UPROPERTY(EditDefaultsOnly, Category="MyPropperties|Widget")
	TSubclassOf<UEndGameWidget> EndGameWidgetType;
	UPROPERTY(EditAnywhere, Category="MyPropperties|DataTable")
	TObjectPtr<UDataTable> DeckDataTable;
	UPROPERTY(EditAnywhere, Category="MyPropperties|Card")
	TObjectPtr<UTexture2D> CardVerso;
	UPROPERTY(EditAnywhere, Category="MyPropperties|Card")
	TObjectPtr<UTexture2D> CopyCardTexture;
	UPROPERTY(EditDefaultsOnly, Category="Sounds")
	TSubclassOf<AAudioContainer> AudioContainerClass;
	
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowGameBoard();
	UFUNCTION(Client, Reliable)
	void ClientRPC_ResetShowGameBoard();
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
	void ServerRPC_ActiveSwitch(uint8 IndexHandCard, EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col,
					EPosition Card2Pos, uint8 Card2Line, uint8 Card2Col);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_ActiveSteal(uint8 IndexHandCard, EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col,
					EPosition Card2Pos, uint8 Card2Line, uint8 Card2Col);
	UFUNCTION(Server, Reliable)
	void ServerRPC_RequestCardData(uint8 Line, uint8 Col);
	UFUNCTION(Client, Reliable)
	void ClientRPC_ReceiveCardDate(FName CardRowName);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_ActiveCopy(FCardDataServer Card, uint8 IndexHandCard, EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col,
					EPosition Card2Pos, uint8 Card2Line, uint8 Card2Col);
	UFUNCTION(Server, Reliable)
	void ServerRPC_EndTurn(ANAFPlayerState* ActivePlayerState);
	UFUNCTION(Server, Reliable)
	void ServerRPC_Replay();


	UFUNCTION(Server, Reliable)
	void ServerRPC_RowNameForCopy(uint8 Line, uint8 Col);
	UFUNCTION(Client, Reliable)
	void ClientRPC_RowNameForCopy(const FName& RowName);

	UFUNCTION(Server, Reliable)
	void ServerRPC_RowNameForSteal(uint8 Line, uint8 Col);
	UFUNCTION(Client, Reliable)
	void ClientRPC_RowNameForSteal(const FName& RowName);

	UFUNCTION(Server, Reliable)
	void ServerRPC_RowNameForSwitch(uint8 Line1, uint8 Col1, uint8 Line2, uint8 Col2);
	UFUNCTION(Client, Reliable)
	void ClientRPC_RowNameForSwitch(const FName& RowName1, const FName& RowName2);
	
	UFUNCTION()
	void UpdateActiveTurnUI(EPosition ActivePosition);
	UFUNCTION()
	void NotifyTurnStart(bool bInIsMyTurn);
	UFUNCTION()
	void EndTurn();
	UFUNCTION()
	void UpdateBoardCard(bool bAfterPlayerAction, const TArray<FName>& InBoardTableRow);
	UFUNCTION()
	void UpdateScores(int32 PLeftScore0,int32 PLeftScore1,int32 PLeftScore2,int32 PLeftTotalScore,
		int32 PRightScore0,int32 PRightScore1,int32 PRightScore2,int32 PRightTotalScore);
	UFUNCTION()
	void ShowEndGamePanel(EPosition WinningPlayerId);
	UFUNCTION()
	void ShowCard(FCardDataServer Card, uint8 Line, uint8 col);

	UFUNCTION()
	void EnableCardSelectionUI(EPosition PlayerId, ECardType CardType);

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
	void ShowCopyCardInHand(uint8 Line, uint8 Col);
	UFUNCTION()
	void HandleCopy(EPosition Card1Pos, uint8 Card1Line, uint8 Card1Col,
					EPosition Card2Pos, uint8 Card2Line, uint8 Card2Col); // Card2 is empty slot

	UFUNCTION()
	void FamilyEffect(uint8 PlayerId, uint8 Line);
	UFUNCTION()
	void NumEffect(TArray<FIntPoint> CoordCardsDeleted);

	UFUNCTION()
	uint8 GetNbCardInBoard() const { return NbCardInBoard; }
	UFUNCTION()
	uint8 GetNbCardInOpponentBoard() const { return NBCardInOpponentBoard; }
	UFUNCTION()
	UTexture2D* GetCopyTexture() const { return CopyCardTexture; }
	UFUNCTION()
	AAudioContainer* GetAudioManager() const { return AudioManager; }

	UFUNCTION(BlueprintCallable)
	bool GetIsMyTurn() const { return bIsMyTurn; }

	UFUNCTION()
	void LaunchAnimPutCard(uint8 Line, uint8 Col);
	UFUNCTION()
	void LaunchAnimCombo2(const TArray<FIntPoint> CoordCardsCombo);
	UFUNCTION()
	void LaunchAnimCombo3(const TArray<FIntPoint> CoordCardsCombo);
	UFUNCTION()
	void RemoveAnimCombo(const TArray<FIntPoint> CoordCardsNotCombo);

	UFUNCTION()
	void Replay();

private:
	bool IsCoordInPlayerIdSide(EPosition PlayerId, uint8 Line, uint8 Col);
	bool IsActivePlayer() const;
	EPosition GetPlayerId() const;
	uint8 NbCardInBoard = 0;
	uint8 NBCardInOpponentBoard = 0;
	TObjectPtr<AAudioContainer> AudioManager;
	FTimerHandle EndTurnHandle;
	FTimerHandle PlaceCardHandle;

	float TimeAnimCardInHand = 1.5f;
	float TimeAnimCardFlipInHand = 0.6f;
};

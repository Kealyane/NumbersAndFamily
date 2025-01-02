// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NumbersAndFamily/NumbersAndFamily.h"
#include "Player/NAFPlayerState.h"
#include "GameWidget.generated.h"


enum class ECardType : uint8;
class UTextBlock;
class UScoreWidget;
class UCardWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSendHandCardSelectedSignature, uint8, CardSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FClickBoardSlotSignature, uint8, Line, uint8, Col);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FActiveSwitchSignature,
											EPosition, Card1Pos, uint8, Card1Line, uint8, Card1Col,
											EPosition, Card2Pos, uint8, Card2Line, uint8, Card2Col);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FActiveStealSignature,
											EPosition, Card1Pos, uint8, Card1Line, uint8, Card1Col,
											EPosition, Card2Pos, uint8, Card2Line, uint8, Card2Col);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FShowCopyCardInHandSignature, uint8, Line, uint8, Col);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FActiveCopySignature,
											EPosition, Card1Pos, uint8, Card1Line, uint8, Card1Col,
											EPosition, Card2Pos, uint8, Card2Line, uint8, Card2Col);

// for blueprint
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayer1IsActiveSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayer1IsInactiveSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayer2IsActiveSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayer2IsInactiveSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetPlayer1NameSignature, FName, PlayerName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetPlayer2NameSignature, FName, PlayerName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLineDestroyedSignature, uint8, PlayerId, uint8, Line);

/**
 * 
 */
UCLASS()
class NUMBERSANDFAMILY_API UGameWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_Deck;
	
	// PLAYER LEFT
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_00;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_01;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_02;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_10;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_11;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_12;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_20;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_21;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_22;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_P1_Pocket;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_P1_Pocket_1;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScoreWidget> WBP_ScoreP1_0;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScoreWidget> WBP_ScoreP1_1;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScoreWidget> WBP_ScoreP1_2;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScoreWidget> WBP_ScoreP1_Total;

	// PLAYER RIGHT
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_03;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_04;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_05;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_13;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_14;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_15;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_23;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_24;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_25;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_P2_Pocket;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_P2_Pocket_1;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScoreWidget> WBP_ScoreP2_0;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScoreWidget> WBP_ScoreP2_1;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScoreWidget> WBP_ScoreP2_2;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScoreWidget> WBP_ScoreP2_Total;

	// STRUCTURES
	
	TArray<TArray<TObjectPtr<UCardWidget>>> BoardSlots;
	TMap<EPosition, TArray<TArray<TObjectPtr<UCardWidget>>>> PlayerBoardSlots;
	TMap<EPosition, TArray<TObjectPtr<UScoreWidget>>> Scores;
	TMap<EPosition, TArray<TObjectPtr<UCardWidget>>> PlayerPockets;
	

	//TTuple<ECardZone,TObjectPtr<UCardWidget>> SelectedCard;
	
	UFUNCTION()
	void ShowActivePlayer(EPosition ActivePlayer);
	UFUNCTION()
	void StartHandSelection(EPosition ActivePlayer);
	UFUNCTION()
	void EndHandSelection(EPosition ActivePlayer);

	UFUNCTION()
	void ActivateHighlight(EPosition PlayerId, ECardType CardType);
	UFUNCTION()
	void DeactivateHighlight();
	
	UFUNCTION()
	void ActiveHandHighlight(EPosition PlayerId);
	UFUNCTION()
	void DeactivateHandHighlight(EPosition PlayerId);
	
	UCardWidget* GetCardWidget(EPosition PlayerPos, uint8 CardPos) const;
	UFUNCTION()
	void SwitchTexture(EPosition PlayerId, uint8 HandSlotIndex, UTexture2D* CardTexture);
	UFUNCTION()
	void UpdateScores(int32 PLeftScore0,int32 PLeftScore1,int32 PLeftScore2,int32 PLeftTotalScore,
		int32 PRightScore0,int32 PRightScore1,int32 PRightScore2,int32 PRightTotalScore);

	UFUNCTION()
	void FamilyEffect(uint8 PlayerId, uint8 Line);
	// EVENTS

	UPROPERTY()
	FSendHandCardSelectedSignature OnSendHandCardSelected;
	UPROPERTY()
	FClickBoardSlotSignature OnClickBoardSlot;
	UPROPERTY()
	FActiveSwitchSignature OnActiveSwitch;
	UPROPERTY()
	FActiveStealSignature OnActiveSteal;
	UPROPERTY()
	FShowCopyCardInHandSignature OnShowCopyCardInHand;
	UPROPERTY()
	FActiveCopySignature OnActiveCopy;

	UPROPERTY(BlueprintAssignable, Category="Player")
	FPlayer1IsActiveSignature Player1IsActive;
	UPROPERTY(BlueprintAssignable, Category="Player")
	FPlayer2IsActiveSignature Player2IsActive;
	UPROPERTY(BlueprintAssignable, Category="Player")
	FPlayer1IsInactiveSignature Player1IsInactive;
	UPROPERTY(BlueprintAssignable, Category="Player")
	FPlayer2IsInactiveSignature Player2IsInactive;
	UPROPERTY(BlueprintAssignable, Category="Player")
	FSetPlayer1NameSignature SetPlayer1Name;
	UPROPERTY(BlueprintAssignable, Category="Player")
	FSetPlayer2NameSignature SetPlayer2Name;
	UPROPERTY(BlueprintAssignable, Category="Game Logic")
	FLineDestroyedSignature LineDestroyed;

protected:
	virtual void NativeConstruct() override;
	void BindCardWidget();
	
private:
	FSlateColor EnableColor = FSlateColor(FColor::Green);
	FSlateColor DisableColor = FSlateColor(FColor::White);

	bool bCanInteractWithBoard = false;
	TObjectPtr<UCardWidget> SelectedHandCard;
	UFUNCTION()
	void OnHandCardSelected(EPosition Player, uint8 LineSelect, uint8 ColSelect);
	UFUNCTION()
	void OnBoardCardSelected(EPosition Player, uint8 LineSelect, uint8 ColSelect);

	EPosition GetPlayerId();
	bool bIsHandChoiceDone = false;

	TObjectPtr<UCardWidget> FirstCardSelected = nullptr;
	TObjectPtr<UCardWidget> SecondCardSelected = nullptr;
	EPosition FirstCardPosition = EPosition::SERVER;
	ECardType HandCardTypeSelected = ECardType::NONE;
	
};

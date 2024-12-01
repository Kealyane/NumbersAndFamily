// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NumbersAndFamily/NumbersAndFamily.h"
#include "GameWidget.generated.h"


enum class ECardType : uint8;
class UTextBlock;
class UScoreWidget;
class UCardWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClickCardSlotSignature, FVector, ButtonPos);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FShowDeckCardSignature, EPosition, PlayerPos, UTexture2D*, CardRecto);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSendHandCardSelectedSignature, uint8, CardSlot);
/**
 * 
 */
UCLASS()
class NUMBERSANDFAMILY_API UGameWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCardWidget> WBP_Card_Deck;
	
	// PLAYER LEFT

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_P1_Name;
	
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
	TObjectPtr<UTextBlock> Text_P2_Name;
	
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
	

	TTuple<ECardZone,TObjectPtr<UCardWidget>> SelectedCard;
	

	UFUNCTION(BlueprintCallable)
	void ButtonCardClicked(FVector ButtonPos);

	UFUNCTION(BlueprintCallable)
	void ShowDeckCard(EPosition PlayerPos, UTexture2D* CardRecto);

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

	// UFUNCTION(BlueprintCallable)
	// void StashArcane();

	void ResetPlayerCardDeck(EPosition PlayerPos);
	UCardWidget* GetCardWidget(EPosition PlayerPos, uint8 CardPos) const;

	// EVENTS
	
	UPROPERTY(BlueprintAssignable)
	FClickCardSlotSignature OnClickCard;
	UPROPERTY(BlueprintAssignable)
	FShowDeckCardSignature OnCardDrawFromDeck;
	UPROPERTY()
	FSendHandCardSelectedSignature OnSendHandCardSelected;

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
};

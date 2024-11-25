// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NumbersAndFamily/NumbersAndFamily.h"
#include "GameWidget.generated.h"


class UTextBlock;
class UScoreWidget;
class UCardWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClickCardSlotSignature, FVector, ButtonPos);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FShowDeckCardSignature, EPosition, PlayerPos, UTexture2D*, CardRecto);
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
	
	UPROPERTY(BlueprintAssignable)
	FClickCardSlotSignature OnClickCard;

	UPROPERTY(BlueprintAssignable)
	FShowDeckCardSignature OnCardDrawFromDeck;

	UFUNCTION(BlueprintCallable)
	void ButtonCardClicked(FVector ButtonPos);

	UFUNCTION(BlueprintCallable)
	void ShowDeckCard(EPosition PlayerPos, UTexture2D* CardRecto);

	UFUNCTION()
	void ShowActivePlayer(EPosition ActivePlayer);

	// UFUNCTION(BlueprintCallable)
	// void StashArcane();

	void ResetPlayerCardDeck(EPosition PlayerPos);
	UCardWidget* GetCardWidget(EPosition PlayerPos, uint8 CardPos) const;

protected:
	virtual void NativeConstruct() override;

private:
	FSlateColor EnableColor = FSlateColor(FColor::Green);
	FSlateColor DisableColor = FSlateColor(FColor::White);
};

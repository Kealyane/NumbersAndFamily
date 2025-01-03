// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NumbersAndFamily/NumbersAndFamily.h"
#include "CardWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCardPlacedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCardRemovedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCardMovedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCardSelectedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCardUnselectedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCardChosenSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCardAllowSelectionSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FClickSignature, EPosition, Player, uint8, LineSelect, uint8, ColSelect);

/**
 * 
 */
UCLASS()
class NUMBERSANDFAMILY_API UCardWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	EPosition OwningPlayer = EPosition::LEFT;
	UPROPERTY(BlueprintReadWrite)
	uint8 Line = 0;
	UPROPERTY(BlueprintReadWrite)
	uint8 Col = 0;

	UPROPERTY(meta=(BindWidget))
	class UButton* ButtonSlot;
	UPROPERTY(meta=(BindWidget))
	class UImage* ImageCard;

	UPROPERTY(BlueprintAssignable)
	FCardPlacedSignature OnCardPlace;
	UPROPERTY(BlueprintAssignable)
	FCardRemovedSignature OnCardRemoved;
	UPROPERTY(BlueprintAssignable)
	FCardMovedSignature OnCardMoved;
	UPROPERTY(BlueprintAssignable)
	FCardSelectedSignature OnCardSelected;
	UPROPERTY(BlueprintAssignable)
	FCardUnselectedSignature OnCardUnselected;
	UPROPERTY(BlueprintAssignable)
	FCardChosenSignature OnCardChosen;
	UPROPERTY(BlueprintAssignable)
	FCardAllowSelectionSignature OnCardAllowSelection;

	UPROPERTY()
	FClickSignature OnClick;

	UFUNCTION(BlueprintCallable)
	void SetSlotParams(bool IsPlayer1, uint8 LineParam, uint8 ColParam);

	// UFUNCTION(BlueprintCallable)
	// void SetSlotCoordParams(uint8 LineParam, uint8 ColParam);

	UFUNCTION(BlueprintCallable)
	void CardClicked(); // Called in BP
	
	// void DeactivateButton(); // disable button
	// void ActivateButton(); // enable button
	
	void ShowCard(UTexture2D* CardImage); // set image texture, card is occupied (true), OnPlaceCard
	void HideCard(); // set alpha to hide image, card is occupied (false)
	void EnableHighlight(); // background button to yellow, can be clicked (true)
	void DisableHighlight(); // background button to white, can be clicked (false)
	void SpecialCardFirstChoiceSelected();
	void SwitchTexture(UTexture2D* CardImage);
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsCardSlotSelected = false;
	
	bool bIsCardOccupied = false;
	bool bCanBeClicked = false;
	bool bIsHandSelectionOnGoing = false;

	void SelectCard(bool bIsSelected);
};

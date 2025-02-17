
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NumbersAndFamily/NumbersAndFamily.h"
#include "CardWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCardPlacedSignature, UTexture2D*, CardImage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCardSelectedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCardUnselectedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHideCardSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetImageSignature, UTexture2D*, CardImage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnableHighlightSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDisableHighlightSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSpecialCardEnableSelectionSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCardDestroyedBySameNumberSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCardDestroyedByFamilyEffectSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FShowCardHandAnimSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPutCardAnimSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombox2AnimSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombox3AnimSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNotAComboSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResetGameSignature);

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

	UPROPERTY(BlueprintAssignable)
	FCardPlacedSignature OnCardPlace;
	UPROPERTY(BlueprintAssignable)
	FCardSelectedSignature OnCardSelected;
	UPROPERTY(BlueprintAssignable)
	FCardUnselectedSignature OnCardUnselected;
	UPROPERTY(BlueprintAssignable)
	FHideCardSignature OnCardHidden;
	UPROPERTY(BlueprintAssignable)
	FSetImageSignature OnSetImage;
	UPROPERTY(BlueprintAssignable)
	FEnableHighlightSignature OnEnableHighlight;
	UPROPERTY(BlueprintAssignable)
	FDisableHighlightSignature OnDisableHighlight;
	UPROPERTY(BlueprintAssignable)
	FSpecialCardEnableSelectionSignature OnSpecialCardEnableSelection;
	UPROPERTY(BlueprintAssignable)
	FCardDestroyedBySameNumberSignature OnCardDestroyBySameNumber;
	UPROPERTY(BlueprintAssignable)
	FCardDestroyedByFamilyEffectSignature OnCardDestroyedByFamily;
	UPROPERTY(BlueprintAssignable)
	FShowCardHandAnimSignature OnShowCardAnim;
	UPROPERTY(BlueprintAssignable)
	FPutCardAnimSignature OnPutCardAnim;
	UPROPERTY(BlueprintAssignable)
	FCombox2AnimSignature OnCombox2Anim;
	UPROPERTY(BlueprintAssignable)
	FCombox3AnimSignature OnCombox3Anim;
	UPROPERTY(BlueprintAssignable)
	FResetGameSignature OnResetGame;
	UPROPERTY(BlueprintAssignable)
	FNotAComboSignature OnNotACombo;
	
	UPROPERTY()
	FClickSignature OnClickSlot;

	UFUNCTION(BlueprintCallable)
	void SetSlotParams(bool IsPlayer1, uint8 LineParam, uint8 ColParam);

	UFUNCTION(BlueprintCallable)
	void CardClicked(); // Called in BP
	
	void ShowCard(UTexture2D* CardImage); // set image texture, card is occupied (true), OnPlaceCard
	void HideCard(); // set alpha to hide image, card is occupied (false)
	void EnableHighlight(); // background button to yellow, can be clicked (true)
	void DisableHighlight(); // background button to white, can be clicked (false)
	void SpecialCardFirstChoiceSelected();
	void SwitchTexture(UTexture2D* CardImage);

	void CardDestroyedByNumEffect();
	void CardDestroyByFamEffect();

	void ShowCardAnim();
	void ResetCard();
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsCardSlotSelected = false;
	UPROPERTY(BlueprintReadOnly)
	bool bIsCardOccupied = false;
	UPROPERTY(BlueprintReadOnly)
	bool bIsHandSlot = false;
	
	bool bCanBeClicked = false;
	bool bIsHandSelectionOnGoing = false;

	void SelectCard(bool bIsSelected);

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameElements/Deck.h"
#include "GameFramework/PlayerState.h"
#include "NumbersAndFamily/NumbersAndFamily.h"
#include "NAFPlayerState.generated.h"

UENUM()
enum class ECardType : uint8
{
	NORMAL,
	SWITCH,
	STEAL,
	COPY,
	NONE
};
namespace EnumCardTypeHelper
{
	static FString ToString(ECardType EnumValue)
	{
		switch (EnumValue)
		{
		case ECardType::SWITCH: return TEXT("SWITCH");
		case ECardType::STEAL: return TEXT("STEAL");
		case ECardType::COPY: return TEXT("COPY");
		case ECardType::NORMAL: return TEXT("NORMAL");
		default: return TEXT("NONE");
		}
	}
}

class ANAFPlayerController;
/**
 * 
 */
UCLASS()
class NUMBERSANDFAMILY_API ANAFPlayerState : public APlayerState
{
	GENERATED_BODY()

	ANAFPlayerState();

public:
	UPROPERTY(ReplicatedUsing=OnRep_Id)
	EPosition Id = EPosition::SERVER;
	UFUNCTION()
	void OnRep_Id();

	void StoreCardInHand(FCardDataServer Card);
	void RemoveCardInHand(uint8 IndexCard);
	void UpdateHandUI(EPosition TargetId, TArray<bool> HandStatus);
	void ActiveHandChoice(EPosition ActiveId);
	
	void UpdateScores(int32 InScoreLine0, int32 InScoreLine1, int32 InScoreLine2, int32 InTotalScore);
	
	ECardType GetCardType(uint8 PosCard);
	FCardDataServer GetSelectedCard();
	uint8 GetIndexSelected() const { return IndexSelected; }
	int32 GetTotalScore() const { return TotalScore; }

	// return true if slot is occupied 
	TArray<bool> HandStatus();
	ANAFPlayerController* GetNafPC() const;

protected:
	UPROPERTY(Replicated)
	FCardDataServer Card1;
	UPROPERTY(Replicated)
	FCardDataServer Card2;

	FCardDataServer SelectedCard;
	uint8 IndexSelected;
	
	UPROPERTY(ReplicatedUsing=OnRep_ScoreLine0)
	int32 ScoreLine0;
	UPROPERTY(ReplicatedUsing=OnRep_ScoreLine1)
	int32 ScoreLine1;
	UPROPERTY(ReplicatedUsing=OnRep_ScoreLine2)
	int32 ScoreLine2;
	UPROPERTY(ReplicatedUsing=OnRep_TotalScore)
	int32 TotalScore;

	UFUNCTION()
	void OnRep_ScoreLine0();
	UFUNCTION()
	void OnRep_ScoreLine1();
	UFUNCTION()
	void OnRep_ScoreLine2();
	UFUNCTION()
	void OnRep_TotalScore();
	
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	
};

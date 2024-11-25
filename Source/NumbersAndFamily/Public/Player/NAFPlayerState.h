// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameElements/Deck.h"
#include "GameFramework/PlayerState.h"
#include "NumbersAndFamily/NumbersAndFamily.h"
#include "NAFPlayerState.generated.h"

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
	void UpdateHandUI(EPosition TargetId, TArray<bool> HandStatus);
	void ActiveTurn(EPosition ActiveId);
	
	// return true if slot is occupied 
	TArray<bool> HandStatus();
	ANAFPlayerController* GetNafPC() const;
	
protected:
	UPROPERTY(Replicated)
	FCardDataServer Card1;
	UPROPERTY(Replicated)
	FCardDataServer Card2;
	
	UPROPERTY(Replicated)
	int32 ScoreLine0;
	UPROPERTY(Replicated)
	int32 ScoreLine1;
	UPROPERTY(Replicated)
	int32 ScoreLine2;
	UPROPERTY(Replicated)
	int32 TotalScore;
	
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	
};

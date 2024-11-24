// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NumbersAndFamily/NumbersAndFamily.h"
#include "ScoreWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FScoreChangedSignature);

/**
 * 
 */
UCLASS()
class NUMBERSANDFAMILY_API UScoreWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	EPosition OwningPlayer = EPosition::LEFT;
	UPROPERTY(BlueprintReadWrite)
	uint8 Line = 0;
	UPROPERTY(BlueprintReadWrite)
	bool bIsTotalScore = false;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* Score;

	UFUNCTION()
	void UpdateScoreText(uint8 ScoreValue);

	UFUNCTION(BlueprintCallable)
	void SetScoreParams(bool IsPlayer1, uint8 LineParam, bool bIsTotalScoreParam);
	UFUNCTION(BlueprintCallable)
	void SetScoreCoordParams(uint8 LineParam, bool bIsTotalScoreParam);
	UFUNCTION(BlueprintCallable)
	void InitScore();
	
	UPROPERTY(BlueprintAssignable)
	FScoreChangedSignature OnScoreUpdated;
};

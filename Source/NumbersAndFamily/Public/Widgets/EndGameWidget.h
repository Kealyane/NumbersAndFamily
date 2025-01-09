// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class NUMBERSANDFAMILY_API UEndGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* EndGameText;

	UFUNCTION(BlueprintCallable)
	void LaunchReplay();

	void SetEndGameText(bool bHasWon, int32 Score);
};

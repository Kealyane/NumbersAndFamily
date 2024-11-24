// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NAFPlayerController.generated.h"

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

protected:
	UPROPERTY(EditDefaultsOnly, Category="Widget")
	TSubclassOf<UGameWidget> GameWidgetType;

public:
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowGameBoard();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NAFGameMode.generated.h"


class ADeck;
/**
 * 
 */
UCLASS()
class NUMBERSANDFAMILY_API ANAFGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Deck")
	TSubclassOf<ADeck> DeckType;
	
	void PostLogin(APlayerController* NewPlayer) override;
	void LaunchGame();

private:
	FTimerHandle WaitHandle;
	TObjectPtr<ADeck> Deck;
};

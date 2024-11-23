// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NAFGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLeaveLobbySignature);
/**
 * 
 */
UCLASS()
class NUMBERSANDFAMILY_API ANAFGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	void PostLogin(APlayerController* NewPlayer) override;

public:
	UPROPERTY(BlueprintAssignable)
	FLeaveLobbySignature OnLeaveLobby;
};

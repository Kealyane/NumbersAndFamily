// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "NAFGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLeaveLobbySignature);

/**
 * 
 */
UCLASS()
class NUMBERSANDFAMILY_API ANAFGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FLeaveLobbySignature OnLeaveLobby;
};

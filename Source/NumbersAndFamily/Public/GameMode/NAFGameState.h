// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "NAFGameState.generated.h"

enum class EPosition : uint8;
class ANAFPlayerState;
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

	ANAFPlayerState* GetOpponentPlayerState(EPosition CurrentId);
};

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

	UPROPERTY(ReplicatedUsing=OnRep_ActiveId)
	EPosition ActiveId;
	UFUNCTION()
	void OnRep_ActiveId();

	UFUNCTION()
	void SetActivePlayer(EPosition InActiveId);
	UFUNCTION(NetMulticast, Unreliable)
	void MultiRPC_PlaySoundStartGame();
	UFUNCTION(NetMulticast, Unreliable)
	void MultiRPC_UpdateActiveTurnUI();

	UFUNCTION()
	void SwitchPlayerTurn();

	ANAFPlayerState* GetOpponentPlayerState(EPosition CurrentId);
	ANAFPlayerState* GetNafPlayerState(EPosition Id);

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void PlaySound(USoundBase* Sound);
};

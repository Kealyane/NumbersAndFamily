// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "NAFGameState.generated.h"

enum class EPosition : uint8;
class ANAFPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLeaveLobbySignature);

UENUM()
enum class EGameStatus : uint8
{
	START,
	IN_GAME,
	END,
};
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

	//UPROPERTY(ReplicatedUsing=OnRep_BoardTableRow)
	UPROPERTY()
	TArray<FName> BoardTableRow;
	// UFUNCTION()
	// void OnRep_BoardTableRow();

	UFUNCTION()
	void SetActivePlayer(EPosition InActiveId);
	UFUNCTION()
	void SetBoardName(const TArray<FName>& InBoardTableRow);
	UFUNCTION()
	void SetStatus(EGameStatus NewStatus) { CurrentStatus = NewStatus; }
	
	UFUNCTION(NetMulticast, Unreliable)
	void MultiRPC_PlaySoundStartGame();
	UFUNCTION(NetMulticast, Unreliable)
	void MultiRPC_UpdateActiveTurnUI(); // Show for both clients which one is active
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_UpdateBoardUI();

	UFUNCTION()
	void SwitchPlayerTurn();
	UFUNCTION()
	void InitBoardRow();


	ANAFPlayerState* GetOpponentPlayerState(EPosition CurrentId);
	ANAFPlayerState* GetNafPlayerState(EPosition Id);

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void PlaySound(USoundBase* Sound);

private:
	EGameStatus CurrentStatus;
};

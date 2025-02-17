// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoundNames.h"
#include "GameFramework/GameState.h"
#include "NAFGameState.generated.h"

struct FCardDataServer;
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
namespace EnumGameStatusHelper
{
	static FString ToString(EGameStatus EnumValue)
	{
		switch (EnumValue)
		{
		case EGameStatus::START: return TEXT("START");
		case EGameStatus::IN_GAME: return TEXT("IN_GAME");
		case EGameStatus::END: return TEXT("END");
		default: return TEXT("Unknown");
		}
	}
}
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
	
	UPROPERTY(Replicated)
	TArray<FName> BoardTableRow;

	UFUNCTION()
	void SetActivePlayer(EPosition InActiveId);
	UFUNCTION()
	void SetBoardName(bool bAfterPlayerAction, const TArray<FName>& InBoardTableRow);
	
	UFUNCTION(NetMulticast, Unreliable)
	void MultiRPC_PlaySoundForBoth(ESoundRow SoundRow);
	UFUNCTION(NetMulticast, Unreliable)
	void MultiRPC_PlayMusicForBoth();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_UpdateActiveTurnUI(EPosition NewActivePlayer); // Show for both clients which one is active
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_UpdateBoardUI(bool bAfterPlayerAction, const TArray<FName>& InBoardTableRow);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_UpdateScores(int32 PLeftScore0,int32 PLeftScore1,int32 PLeftScore2,int32 PLeftTotalScore,
		int32 PRightScore0,int32 PRightScore1,int32 PRightScore2,int32 PRightTotalScore);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_EndGame(EPosition InWinner);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_FamilyEffect(uint8 PlayerID, uint8 Line);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_NumEffect(const TArray<FIntPoint>& CoordCardsDeleted);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_PutCard(uint8 Line, uint8 Col);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_ShowCard(FCardDataServer Card, uint8 Line, uint8 Col);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_Combo2(const TArray<FIntPoint>& CoordCardsCombo);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_Combo3(const TArray<FIntPoint>& CoordCardsCombo);
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_CardsNotCombo(const TArray<FIntPoint>& CoordCardsNotCombo);
	
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPC_SetPlayerNames(FName Player1, FName Player2);

	UFUNCTION()
	void SetPlayerNames();
	UFUNCTION()
	void SwitchPlayerTurn();
	UFUNCTION()
	void InitBoardRow();
	UFUNCTION()
	void UpdateScores(int32 PLeftScore0,int32 PLeftScore1,int32 PLeftScore2,int32 PLeftTotalScore,
		int32 PRightScore0,int32 PRightScore1,int32 PRightScore2,int32 PRightTotalScore);
	UFUNCTION()
	void SetScoresServer(int32 PLeftScore0,int32 PLeftScore1,int32 PLeftScore2,
		int32 PRightScore0,int32 PRightScore1,int32 PRightScore2);

	ANAFPlayerState* GetOpponentPlayerState(EPosition CurrentId);
	ANAFPlayerState* GetNafPlayerState(EPosition Id);

	int32 P1Score0;
	int32 P1Score1;
	int32 P1Score2;
	int32 P2Score0;
	int32 P2Score1;
	int32 P2Score2;
protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};

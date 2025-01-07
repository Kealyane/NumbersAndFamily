// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NAFGameState.h"

#include "SoundNames.h"
#include "GameElements/AudioContainer.h"
#include "Net/UnrealNetwork.h"
#include "Player/NAFPlayerController.h"
#include "Player/NAFPlayerState.h"

void ANAFGameState::OnRep_ActiveId()
{
}

void ANAFGameState::SetActivePlayer(EPosition InActiveId)
{
	ActiveId = InActiveId;
	MultiRPC_UpdateActiveTurnUI(InActiveId);
}

void ANAFGameState::SetBoardName(bool bAfterPlayerAction, const TArray<FName>& InBoardTableRow)
{
	if (HasAuthority())
	{
		BoardTableRow = InBoardTableRow;
		MultiRPC_UpdateBoardUI(bAfterPlayerAction, InBoardTableRow);
	}
}

void ANAFGameState::MultiRPC_PlayMusicForBoth_Implementation()
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		PlayerController->GetAudioManager()->PlayMusic();
	}
}

void ANAFGameState::MultiRPC_PlaySoundForBoth_Implementation(ESoundRow SoundRow)
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		PlayerController->GetAudioManager()->PlayAudio(SoundRow);
	}
}

void ANAFGameState::MultiRPC_UpdateActiveTurnUI_Implementation(EPosition NewActivePlayer)
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		if (ANAFPlayerState* PlayerState = PlayerController->GetPlayerState<ANAFPlayerState>())
		{
			PlayerController->NotifyTurnStart(PlayerState->Id == NewActivePlayer);
		}
		PlayerController->UpdateActiveTurnUI(NewActivePlayer);
	}
}

void ANAFGameState::MultiRPC_UpdateBoardUI_Implementation(bool bAfterPlayerAction, const TArray<FName>& InBoardTableRow)
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		PlayerController->UpdateBoardCard(bAfterPlayerAction, InBoardTableRow);
	}
}

void ANAFGameState::MultiRPC_UpdateScores_Implementation(int32 PLeftScore0, int32 PLeftScore1, int32 PLeftScore2,
	int32 PLeftTotalScore, int32 PRightScore0, int32 PRightScore1, int32 PRightScore2, int32 PRightTotalScore)
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		PlayerController->UpdateScores(PLeftScore0, PLeftScore1, PLeftScore2, PLeftTotalScore,
			PRightScore0, PRightScore1, PRightScore2, PRightTotalScore);
	}
}

void ANAFGameState::MultiRPC_EndGame_Implementation(EPosition InWinner)
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		PlayerController->ShowEndGamePanel(InWinner);
	}
}

void ANAFGameState::MultiRPC_FamilyEffect_Implementation(uint8 PlayerID, uint8 Line)
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		PlayerController->FamilyEffect(PlayerID, Line);
	}
}

void ANAFGameState::MultiRPC_NumEffect_Implementation(const TArray<FIntPoint>& CoordCardsDeleted)
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		PlayerController->NumEffect(CoordCardsDeleted);
	}
}

void ANAFGameState::MultiRPC_PutCard_Implementation(uint8 Line, uint8 Col)
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		PlayerController->LaunchAnimPutCard(Line, Col);
	}
}

void ANAFGameState::MultiRPC_Combo2_Implementation(const TArray<FIntPoint>& CoordCardsCombo)
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		PlayerController->LaunchAnimCombo2(CoordCardsCombo);
	}
}

void ANAFGameState::MultiRPC_Combo3_Implementation(const TArray<FIntPoint>& CoordCardsCombo)
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		PlayerController->LaunchAnimCombo3(CoordCardsCombo);
	}
}

void ANAFGameState::MultiRPC_ShowCard_Implementation(FCardDataServer Card, uint8 Line, uint8 Col)
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		PlayerController->ShowCard(Card, Line, Col);
	}
}

void ANAFGameState::MultiRPC_SetPlayerNames_Implementation(FName Player1, FName Player2)
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		if (PlayerController->GetPlayerId() == EPosition::LEFT)
		{
			FString StringName1 = Player1.ToString();
			FString NewString1 = TEXT("x ") + StringName1 + TEXT(" x");
			FName NewName1 = FName(*NewString1);
			PlayerController->SetPlayerNames(NewName1, Player2);
		}
		else
		{
			FString StringName2 = Player2.ToString();
			FString NewString2 = TEXT("X ") + StringName2 + TEXT(" X");
			FName NewName2 = FName(*NewString2);
			PlayerController->SetPlayerNames(Player1, NewName2);
		}
	}
}

void ANAFGameState::SetPlayerNames()
{
	ANAFPlayerState* Player1 = GetNafPlayerState(EPosition::LEFT);
	ANAFPlayerState* Player2 = GetNafPlayerState(EPosition::RIGHT);
	if (Player1 && Player2)
	{
		MultiRPC_SetPlayerNames(Player1->PlayerAlias, Player2->PlayerAlias);
	}
}

void ANAFGameState::SwitchPlayerTurn()
{
	if (HasAuthority())
	{
		ActiveId = ActiveId == EPosition::LEFT ? EPosition::RIGHT : EPosition::LEFT;
		MultiRPC_UpdateActiveTurnUI(ActiveId);
	}
}

void ANAFGameState::InitBoardRow()
{
	if (HasAuthority())
	{
		BoardTableRow.Init(FName("NONE"), 18);
		P1Score0 = 0;
		P1Score1 = 0;
		P1Score2 = 0;
		P2Score0 = 0;
		P2Score1 = 0;
		P2Score2 = 0;
		MultiRPC_UpdateBoardUI(false, BoardTableRow);
	}
}

void ANAFGameState::UpdateScores(int32 PLeftScore0, int32 PLeftScore1, int32 PLeftScore2, int32 PLeftTotalScore,
	int32 PRightScore0, int32 PRightScore1, int32 PRightScore2, int32 PRightTotalScore)
{
	ANAFPlayerState* PlayerLeft = GetNafPlayerState(EPosition::LEFT);
	if (PlayerLeft) PlayerLeft->UpdateScores(PLeftScore0, PLeftScore1, PLeftScore2, PLeftTotalScore);
	ANAFPlayerState* PlayerRight = GetNafPlayerState(EPosition::RIGHT);
	if (PlayerRight) PlayerRight->UpdateScores(PRightScore0, PRightScore1, PRightScore2, PRightTotalScore);
}

void ANAFGameState::SetScoresServer(int32 PLeftScore0, int32 PLeftScore1, int32 PLeftScore2,
	int32 PRightScore0, int32 PRightScore1, int32 PRightScore2)
{
	P1Score0 = PLeftScore0;
	P1Score1 = PLeftScore1;
	P1Score2 = PLeftScore2;
	P2Score0 = PRightScore0;
	P2Score1 = PRightScore1;
	P2Score2 = PRightScore2;
}

ANAFPlayerState* ANAFGameState::GetOpponentPlayerState(EPosition CurrentId)
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (ANAFPlayerState* NafPS = Cast<ANAFPlayerState>(PlayerState))
		{
			if (NafPS->Id != CurrentId)
			{
				return NafPS;
			}
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Game State : NO Opponent found"));
	return nullptr;
}

ANAFPlayerState* ANAFGameState::GetNafPlayerState(EPosition Id)
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (ANAFPlayerState* NafPS = Cast<ANAFPlayerState>(PlayerState))
		{
			if (NafPS->Id == Id)
			{
				return NafPS;
			}
		}
	}
	return nullptr;
}

void ANAFGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANAFGameState, ActiveId);
	DOREPLIFETIME(ANAFGameState, BoardTableRow);
}

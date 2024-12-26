// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NAFGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/NAFPlayerController.h"
#include "Player/NAFPlayerState.h"

void ANAFGameState::OnRep_ActiveId()
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		if (ANAFPlayerState* PlayerState = PlayerController->GetPlayerState<ANAFPlayerState>())
		{
			PlayerController->NotifyTurnStart(PlayerState->Id == ActiveId);
		}
	}
	MultiRPC_UpdateActiveTurnUI();
}

void ANAFGameState::SetActivePlayer(EPosition InActiveId)
{
	if (HasAuthority())
	{
		ActiveId = InActiveId;
		if (IsNetMode(NM_ListenServer))
			OnRep_ActiveId();
	}
}

void ANAFGameState::SetBoardName(bool bAfterPlayerAction, const TArray<FName>& InBoardTableRow)
{
	if (HasAuthority())
	{
		BoardTableRow = InBoardTableRow;
		MultiRPC_UpdateBoardUI(bAfterPlayerAction, InBoardTableRow);
	}
}

void ANAFGameState::MultiRPC_PlaySoundStartGame_Implementation()
{
	// TODO : replace nullptr by sound
	PlaySound(nullptr);
}

void ANAFGameState::MultiRPC_UpdateActiveTurnUI_Implementation()
{
	ANAFPlayerController* PlayerController = GetWorld()->GetFirstPlayerController<ANAFPlayerController>();
	if (PlayerController)
	{
		PlayerController->UpdateActiveTurnUI(ActiveId);
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

void ANAFGameState::SwitchPlayerTurn()
{
	if (HasAuthority())
	{
		ActiveId = ActiveId == EPosition::LEFT ? EPosition::RIGHT : EPosition::LEFT;
		OnRep_ActiveId();
	}
}

void ANAFGameState::InitBoardRow()
{
	if (HasAuthority())
	{
		BoardTableRow.Init(FName("NONE"), 18);
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

void ANAFGameState::PlaySound(USoundBase* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(this, Sound);
	}
}

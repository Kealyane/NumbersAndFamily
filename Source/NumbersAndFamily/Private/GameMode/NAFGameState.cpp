// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NAFGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/NAFPlayerController.h"
#include "Player/NAFPlayerState.h"

void ANAFGameState::OnRep_ActiveId()
{
	UE_LOG(LogTemp, Warning, TEXT("game state : OnRep_ActiveId"));
	if (ANAFPlayerState* PS = GetNafPlayerState(ActiveId))
	{
		if (ANAFPlayerController* PC = PS->GetNafPC())
		{
			PC->NotifyTurnStart(true);
		}
	}
	if (ANAFPlayerState* PS = GetOpponentPlayerState(ActiveId))
	{
		if (ANAFPlayerController* PC = PS->GetNafPC())
		{
			PC->NotifyTurnStart(false);
		}
	}
}

void ANAFGameState::SetActivePlayer(EPosition InActiveId)
{
	if (HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Magenta, FString(TEXT("game state : SetActivePlayer")));
		UE_LOG(LogTemp, Warning, TEXT("game state : SetActivePlayer"));
		ActiveId = InActiveId;
		if (IsNetMode(NM_ListenServer))
			OnRep_ActiveId();
	}
}

void ANAFGameState::SetBoardName(bool bAfterPlayerAction, const TArray<FName>& InBoardTableRow)
{
	if (HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Magenta, FString(TEXT("game state : SetBoardName")));
		UE_LOG(LogTemp, Warning, TEXT("game state : SetBoardName"));
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
		UE_LOG(LogTemp, Warning, TEXT("game state : MultiRPC_UpdateBoardUI_Implementation"));
		PlayerController->UpdateBoardCard(bAfterPlayerAction, InBoardTableRow);
	}
}

void ANAFGameState::SwitchPlayerTurn()
{
	if (HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Magenta, FString(TEXT("game state : SwitchPlayerTurn")));
		UE_LOG(LogTemp, Warning, TEXT("game state : SwitchPlayerTurn"));
		ActiveId = ActiveId == EPosition::LEFT ? EPosition::RIGHT : EPosition::LEFT;
		OnRep_ActiveId();
	}
}

void ANAFGameState::InitBoardRow()
{
	UE_LOG(LogTemp, Warning, TEXT("game state : InitBoardRow"));
	if (HasAuthority())
	{
		BoardTableRow.Init(FName("NONE"), 18);
		MultiRPC_UpdateBoardUI(false, BoardTableRow);
	}
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

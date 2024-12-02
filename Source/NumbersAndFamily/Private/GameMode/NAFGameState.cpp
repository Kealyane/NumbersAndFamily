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
			PC->NotifyTurnStart();
		}
	}
}

// void ANAFGameState::OnRep_BoardTableRow()
// {
// 	//GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Magenta, FString(TEXT("game state : OnRep_BoardTableRow")));
// 	UE_LOG(LogTemp, Warning, TEXT("game state : OnRep_BoardTableRow"));
// 	MultiRPC_UpdateBoardUI();
// }

void ANAFGameState::SetActivePlayer(EPosition InActiveId)
{
	if (HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Magenta, FString(TEXT("game state : SetActivePlayer")));
		UE_LOG(LogTemp, Warning, TEXT("game state : SetActivePlayer"));
		ActiveId = InActiveId;
		OnRep_ActiveId();
	}
}

void ANAFGameState::SetBoardName(const TArray<FName>& InBoardTableRow)
{
	if (HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Magenta, FString(TEXT("game state : SetBoardName")));
		UE_LOG(LogTemp, Warning, TEXT("game state : SetBoardName"));
		BoardTableRow = InBoardTableRow;
		MultiRPC_UpdateBoardUI();
		//OnRep_BoardTableRow();
	}
}

void ANAFGameState::MultiRPC_PlaySoundStartGame_Implementation()
{
	// TODO : replace nullptr by sound
	PlaySound(nullptr);
}

void ANAFGameState::MultiRPC_UpdateActiveTurnUI_Implementation()
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (ANAFPlayerState* NafPS = Cast<ANAFPlayerState>(PlayerState))
		{
			if (ANAFPlayerController* NafPC = NafPS->GetNafPC())
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Magenta, FString(TEXT("game state : MultiRPC_UpdateActiveTurnUI")));
				UE_LOG(LogTemp, Warning, TEXT("game state : MultiRPC_UpdateActiveTurnUI_Implementation %s"), *EnumHelper::ToString(NafPS->Id));
				NafPC->UpdateActiveTurnUI(ActiveId);
			}
		}
	}
}

void ANAFGameState::MultiRPC_UpdateBoardUI_Implementation()
{

	UE_LOG(LogTemp, Warning, TEXT("game state : MultiRPC_UpdateBoardUI_Implementation %d"),PlayerArray.Num());
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (ANAFPlayerState* NafPS = Cast<ANAFPlayerState>(PlayerState))
		{
			if (ANAFPlayerController* NafPC = NafPS->GetNafPC())
			{
				//GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Magenta, FString::Printf(TEXT("game state : MultiRPC_UpdateBoardUI %s table row %d"), *EnumHelper::ToString(NafPS->Id), BoardTableRow.Num()));
				UE_LOG(LogTemp, Warning, TEXT("game state : MultiRPC_UpdateBoardUI_Implementation %s"),*EnumHelper::ToString(NafPS->Id));
				
				// NafPC->UpdateBoardCard(BoardTableRow);
				// if (NafPS->Id == ActiveId)
				// {
				// 	if (CurrentStatus == EGameStatus::IN_GAME) NafPC->EndTurn();
				// }
			}
		}
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
	BoardTableRow.Init(FName("NONE"), 18);
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
	//DOREPLIFETIME(ANAFGameState, BoardTableRow);
}

void ANAFGameState::PlaySound(USoundBase* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(this, Sound);
	}
}

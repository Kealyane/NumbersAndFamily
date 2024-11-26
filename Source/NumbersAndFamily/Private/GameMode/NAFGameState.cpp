// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NAFGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/NAFPlayerController.h"
#include "Player/NAFPlayerState.h"

void ANAFGameState::OnRep_ActiveId()
{
	if (ANAFPlayerState* PS = GetNafPlayerState(ActiveId))
	{
		if (ANAFPlayerController* PC = PS->GetNafPC())
		{
			PC->NotifyTurnStart();
		}
	}
}

void ANAFGameState::SetActivePlayer(EPosition InActiveId)
{
	if (HasAuthority())
	{
		ActiveId = InActiveId;
		OnRep_ActiveId();
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
				NafPC->UpdateActiveTurnUI(ActiveId);
			}
		}
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

ANAFPlayerState* ANAFGameState::GetOpponentPlayerState(EPosition CurrentId)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Magenta,
			FString::Printf(TEXT("Game State : Get Opponent")));
	}
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
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Magenta,
			FString::Printf(TEXT("Game State : NO Opponent found")));
	}
	return nullptr;
}

ANAFPlayerState* ANAFGameState::GetNafPlayerState(EPosition Id)
{
	for (APlayerState* PlayerState : PlayerArray)
	{
		if (ANAFPlayerState* NafPS = Cast<ANAFPlayerState>(PlayerState))
		{
			if (NafPS->Id != Id)
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
}

void ANAFGameState::PlaySound(USoundBase* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(this, Sound);
	}
}

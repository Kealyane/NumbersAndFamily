// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NAFPlayerState.h"

#include "Net/UnrealNetwork.h"

ANAFPlayerState::ANAFPlayerState()
{
}

void ANAFPlayerState::OnRep_Id()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow,
			FString::Printf(TEXT("tarot player state : player %s"), *EnumHelper::ToString(Id)));
	}
}

void ANAFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANAFPlayerState, Id);
}

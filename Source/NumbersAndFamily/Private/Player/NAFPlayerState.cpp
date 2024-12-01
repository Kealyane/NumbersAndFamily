// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NAFPlayerState.h"

#include "GameMode/NAFGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/NAFPlayerController.h"

ANAFPlayerState::ANAFPlayerState()
{
	ScoreLine0 = 0;
	ScoreLine1 = 0;
	ScoreLine2 = 0;
	TotalScore = 0;
	Card1 = FCardDataServer();
	Card2 = FCardDataServer();
}

void ANAFPlayerState::OnRep_Id()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow,
			FString::Printf(TEXT("Player State : player %s"), *EnumHelper::ToString(Id)));
	}
}

void ANAFPlayerState::StoreCardInHand(FCardDataServer Card)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow,
			FString::Printf(TEXT("Player State %s : Store Card"), *EnumHelper::ToString(Id)));
	}
	uint8 CardPos = 0;
	if (Card1.RowName.IsNone())
	{
		Card1 = Card;
		CardPos = 1;
	}
	else if (Card2.RowName.IsNone())
	{
		Card2 = Card;
		CardPos = 2;
	}
	if (ANAFPlayerController* NafPC = GetNafPC())
	{
		NafPC->ClientRPC_PlaceCardInPocketUI(Id,CardPos,Card.RowName);
	}
}

void ANAFPlayerState::UpdateHandUI(EPosition TargetId, TArray<bool> HandStatus)
{
	if (ANAFPlayerController* NafPC = GetNafPC())
	{
		if (HandStatus[0])
		{
			NafPC->ClientRPC_ShowPocketCardVerso(TargetId, 1);
		}
		if (HandStatus[1])
		{
			NafPC->ClientRPC_ShowPocketCardVerso(TargetId, 2);
		}
	}
}

void ANAFPlayerState::ActiveTurn(EPosition ActiveId)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow,
			FString::Printf(TEXT("Player State %s : Your turn"), *EnumHelper::ToString(Id)));
	}
}

ECardType ANAFPlayerState::GetCardType(uint8 PosCard)
{
	if (PosCard == 1)
	{
		if (Card1.RowName.IsNone()) return ECardType::NONE;
		if (Card1.FamilyType != EFamilyType::NONE) return ECardType::NORMAL;
		if (Card1.ArcaneType == EArcaneType::COPY) return ECardType::COPY;
		if (Card1.ArcaneType == EArcaneType::SWITCH) return ECardType::SWITCH;
		if (Card1.ArcaneType == EArcaneType::STEAL) return ECardType::STEAL;
	}
	else if (PosCard == 2)
	{
		if (Card2.RowName.IsNone()) return ECardType::NONE;
		if (Card2.FamilyType != EFamilyType::NONE) return ECardType::NORMAL;
		if (Card2.ArcaneType == EArcaneType::COPY) return ECardType::COPY;
		if (Card2.ArcaneType == EArcaneType::SWITCH) return ECardType::SWITCH;
		if (Card2.ArcaneType == EArcaneType::STEAL) return ECardType::STEAL;
	}
	return ECardType::NONE;
}

void ANAFPlayerState::ActiveHandChoice(EPosition ActiveId)
{
	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow,
	FString::Printf(TEXT("Player State %s : ActiveHandChoice %s"), *EnumHelper::ToString(Id), *EnumHelper::ToString(ActiveId)));
	if (ANAFPlayerController* NafPC = GetNafPC())
	{
		NafPC->ClientRPC_ActiveHand(ActiveId);
	}
}

TArray<bool> ANAFPlayerState::HandStatus()
{
	TArray<bool> Status;
	Status.Add(!Card1.RowName.IsNone());
	Status.Add(!Card2.RowName.IsNone());
	return Status;
}

void ANAFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANAFPlayerState, Id);
	DOREPLIFETIME(ANAFPlayerState, ScoreLine0);
	DOREPLIFETIME(ANAFPlayerState, ScoreLine1);
	DOREPLIFETIME(ANAFPlayerState, ScoreLine2);
	DOREPLIFETIME(ANAFPlayerState, TotalScore);
	DOREPLIFETIME(ANAFPlayerState, Card1);
	DOREPLIFETIME(ANAFPlayerState, Card2);
}


ANAFPlayerController* ANAFPlayerState::GetNafPC() const
{
	//return Cast<ANAFPlayerController>(GetPlayerController());
	return Cast<ANAFPlayerController>(GetOwner());
}


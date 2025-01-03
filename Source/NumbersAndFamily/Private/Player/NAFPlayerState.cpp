// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NAFPlayerState.h"
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
}

void ANAFPlayerState::StoreCardInHand(FCardDataServer Card)
{
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
		NafPC->ClientRPC_PlaceCardInPocketUI(Id, CardPos, Card.RowName);
		// GetWorld()->GetTimerManager().SetTimer(PSPlaceCardHandle,
		// 	[this,NafPC,CardPos, Card]
		// {
		// },
		// 3.f,
		// false);
	}
}

void ANAFPlayerState::RemoveCardInHand(uint8 IndexCard)
{
	if (IndexCard == 1)
	{
		Card1.ResetCard();
	}
	else if (IndexCard == 2)
	{
		Card2.ResetCard();
	}
	if (ANAFPlayerController* NafPC = GetNafPC())
	{
		NafPC->ClientRPC_PocketCardEmpty(Id, IndexCard);
	}
}

void ANAFPlayerState::UpdateHandUI(EPosition TargetId, TArray<bool> HandStatus)
{
	if (ANAFPlayerController* NafPC = GetNafPC())
	{
		if (HandStatus[0])
		{
			GetWorld()->GetTimerManager().SetTimer(PSPlaceCardVersoHandle,
				[this, NafPC, TargetId]()
				{
					NafPC->ClientRPC_ShowPocketCardVerso(TargetId, 1);
				},
				3.3f,
				false);
		}
		else
		{
			NafPC->ClientRPC_PocketCardEmpty(TargetId, 1);
		}
		if (HandStatus[1])
		{
			GetWorld()->GetTimerManager().SetTimer(PSPlaceCardVersoHandle,
				[this, NafPC, TargetId]()
				{
					NafPC->ClientRPC_ShowPocketCardVerso(TargetId, 2);
				},
				3.3f,
				false);

		}
		else
		{
			NafPC->ClientRPC_PocketCardEmpty(TargetId, 2);
		}
	}
}

ECardType ANAFPlayerState::GetCardType(uint8 PosCard)
{

	if (PosCard == 1)
	{
		SelectedCard = Card1;
		IndexSelected = 1;

		if (Card1.RowName.IsNone()) return ECardType::NONE;
		if (Card1.FamilyType != EFamilyType::NONE) return ECardType::NORMAL;
		if (Card1.ArcaneType == EArcaneType::COPY) return ECardType::COPY;
		if (Card1.ArcaneType == EArcaneType::SWITCH) return ECardType::SWITCH;
		if (Card1.ArcaneType == EArcaneType::STEAL) return ECardType::STEAL;
	}
	else if (PosCard == 2)
	{
		SelectedCard = Card2;
		IndexSelected = 2;

		if (Card2.RowName.IsNone()) return ECardType::NONE;
		if (Card2.FamilyType != EFamilyType::NONE) return ECardType::NORMAL;
		if (Card2.ArcaneType == EArcaneType::COPY) return ECardType::COPY;
		if (Card2.ArcaneType == EArcaneType::SWITCH) return ECardType::SWITCH;
		if (Card2.ArcaneType == EArcaneType::STEAL) return ECardType::STEAL;
	}
	SelectedCard = FCardDataServer();
	IndexSelected = 0;
	return ECardType::NONE;
}

FCardDataServer ANAFPlayerState::GetSelectedCard()
{
	return SelectedCard;
}

void ANAFPlayerState::ActiveHandChoice(EPosition ActiveId)
{
	if (ANAFPlayerController* NafPC = GetNafPC())
	{
		NafPC->ClientRPC_ActiveHand(ActiveId);
	}
}

void ANAFPlayerState::UpdateScores(int32 InScoreLine0, int32 InScoreLine1, int32 InScoreLine2, int32 InTotalScore)
{
	if (HasAuthority())
	{
		ScoreLine0 = InScoreLine0;
		ScoreLine1 = InScoreLine1;
		ScoreLine2 = InScoreLine2;
		TotalScore = InTotalScore;
	}
}

TArray<bool> ANAFPlayerState::HandStatus()
{
	TArray<bool> Status;
	Status.Add(!Card1.RowName.IsNone());
	Status.Add(!Card2.RowName.IsNone());
	return Status;
}

void ANAFPlayerState::OnRep_ScoreLine0()
{
}

void ANAFPlayerState::OnRep_ScoreLine1()
{
}

void ANAFPlayerState::OnRep_ScoreLine2()
{
}

void ANAFPlayerState::OnRep_TotalScore()
{
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
	return Cast<ANAFPlayerController>(GetPlayerController());
}


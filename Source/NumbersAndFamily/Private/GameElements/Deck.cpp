// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/Deck.h"
#include "Algo/RandomShuffle.h"

ADeck::ADeck()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADeck::InitDeck()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Cyan,
			FString::Printf(TEXT("Deck : Init Deck")));
	}
	
	TArray<FName> RowNames = DeckDataTable->GetRowNames();
	const FString ContextString(TEXT("Tower Data Context"));

	for (const FName RowName : RowNames)
	{
		const FCardData* Data = DeckDataTable->FindRow<FCardData>(RowName,ContextString);
		Deck.Add(FCardDataServer(
			RowName,
			Data->FamilyType,
			Data->Score,
			Data->ArcaneType));
	}
}

FCardDataServer ADeck::DrawCard()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Cyan,
			FString::Printf(TEXT("Deck : Draw Card")));
	}
	
	Algo::RandomShuffle(Deck);
	const FCardDataServer CardDrawn = Deck[0];
	Deck.Remove(CardDrawn);
	return CardDrawn;
}

void ADeck::BackToDeck(FCardDataServer Card)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Cyan,
			FString::Printf(TEXT("Deck : Card back to deck")));
	}
	Deck.Add(Card);
}





// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/Deck.h"
#include "Algo/RandomShuffle.h"

ADeck::ADeck()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ADeck::InitDeck()
{
	if (DeckDataTable == nullptr || DeckDataTable->GetRowMap().Num() == 0)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Red,
				FString::Printf(TEXT("Deck : Data Table DT_Deck is empty, you have to initialize it")));
			return false;
		}
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
	bIsStartGame = true;
	return true;
}

FCardDataServer ADeck::DrawCard()
{
	if (Deck.Num() == 0)
	{
		return FCardDataServer();
	}
	FCardDataServer CardDrawn;

	if (bIsStartGame)
	{
		do
		{
			Algo::RandomShuffle(Deck);
			CardDrawn = Deck[0];
		}
		while (CardDrawn.ArcaneType != EArcaneType::NONE);
		bIsStartGame = false;
	}
	else
	{
		Algo::RandomShuffle(Deck);
		CardDrawn = Deck[0];
	}

	// Do not remove special cards
	if (CardDrawn.ArcaneType == EArcaneType::NONE)
	{
		Deck.Remove(CardDrawn);
	}
	return CardDrawn;
}

void ADeck::BackToDeck(FCardDataServer Card)
{
	if (Deck.Contains(Card))
	{
		UE_LOG(LogTemp, Warning, TEXT("Deck : Card already in deck"));
		return;
	}
	Deck.Add(Card);
}

FName ADeck::GetRowNameFromDataServer(FCardDataServer Card)
{
	if (DeckDataTable)
	{
		TMap<FName, uint8*> RowMap = DeckDataTable->GetRowMap();
		
		for (const TPair<FName, uint8*>& Row : RowMap)
		{
			FCardData* CardData = reinterpret_cast<FCardData*>(Row.Value);
			if (CardData && CardData->FamilyType == Card.FamilyType && CardData->Score == Card.Score)
			{
				return Row.Key;
			}
		}
	}
	return FName("NONE");
}





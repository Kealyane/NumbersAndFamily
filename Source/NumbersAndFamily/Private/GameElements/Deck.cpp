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
	return true;
}

FCardDataServer ADeck::DrawCard()
{
	UE_LOG(LogTemp, Warning, TEXT("Deck draw card : length %d"), Deck.Num());
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Cyan,
	// 		FString::Printf(TEXT("Deck : Draw Card")));
	// }
	if (Deck.Num() == 0)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Deck draw card : 0"));
		return FCardDataServer();
	}
	Algo::RandomShuffle(Deck);
	FCardDataServer CardDrawn = Deck[0];
	//CardDrawn.DebugCard("Deck Draw Card Value");
	if (CardDrawn.ArcaneType == EArcaneType::NONE)
	{
		Deck.Remove(CardDrawn);
	}
	return CardDrawn;
}

void ADeck::BackToDeck(FCardDataServer Card)
{
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Cyan,
	// 		FString::Printf(TEXT("Deck : Card back to deck")));
	// }
	//UE_LOG(LogTemp, Warning, TEXT("Deck BackToDeck : deck length %d"), Deck.Num());
	if (Deck.Contains(Card))
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Red,
				FString::Printf(TEXT("Deck : Card already in deck")));
		return;
	}
	Deck.Add(Card);
	//UE_LOG(LogTemp, Warning, TEXT("Deck BackToDeck : deck length after adding card %d"), Deck.Num());
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





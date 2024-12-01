// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Deck.generated.h"

UENUM(BlueprintType)
enum class EFamilyType : uint8
{
	NONE,
	CROW,
	DEER,
	CAT,
	WOLF,
};

UENUM(BlueprintType)
enum class EArcaneType : uint8
{
	NONE,
	LOVERS,
	FOOL,
	NAMELESS,
};

namespace EnumCardHelper
{
	static FString FamilyToString(EFamilyType EnumValue)
	{
		switch (EnumValue)
		{
		case EFamilyType::CROW: return TEXT("CROW");
		case EFamilyType::CAT: return TEXT("CAT");
		case EFamilyType::WOLF: return TEXT("WOLF");
		case EFamilyType::DEER: return TEXT("DEER");
		default: return TEXT("NONE");
		}
	}
	static FString ArcaneToString(EArcaneType EnumValue)
	{
		switch (EnumValue)
		{
		case EArcaneType::FOOL: return TEXT("FOOL");
		case EArcaneType::LOVERS: return TEXT("LOVERS");
		case EArcaneType::NAMELESS: return TEXT("NAMELESS");
		default: return TEXT("NONE");
		}
	}
}

USTRUCT(BlueprintType)
struct FCardData : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Card/Datas")
	EFamilyType FamilyType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Card/Datas", meta=(ClampMin="0", ClampMax="6"))
	int32 Score;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Card/Datas/Arcane")
	EArcaneType ArcaneType = EArcaneType::NONE;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Card/Textures")
	TObjectPtr<UTexture2D> ImageRecto;
};

USTRUCT()
struct FCardDataServer
{
	GENERATED_BODY()

	UPROPERTY()
	FName RowName;
	UPROPERTY()
	EFamilyType FamilyType;
	UPROPERTY()
	int32 Score;
	UPROPERTY()
	EArcaneType ArcaneType;

	FCardDataServer() :
	RowName(FName("NONE")), FamilyType(EFamilyType::NONE), Score(0), ArcaneType(EArcaneType::NONE) {}
	
	FCardDataServer(FName InRowName, EFamilyType InFamilyType, int32 InScore, EArcaneType InArcaneType) :
	RowName(InRowName), FamilyType(InFamilyType), Score(InScore), ArcaneType(InArcaneType) {}

	void ResetCard()
	{
		RowName = FName("NONE");
		FamilyType = EFamilyType::NONE;
		Score = 0;
		ArcaneType = EArcaneType::NONE;
	}
	
	void SetCard(FCardDataServer NewCard)
	{
		RowName = NewCard.RowName;
		FamilyType = NewCard.FamilyType;
		Score = NewCard.Score;
		ArcaneType = NewCard.ArcaneType;
	}

	bool operator==(const FCardDataServer& Other) const
	{
		return RowName == Other.RowName &&
			   FamilyType == Other.FamilyType &&
			   Score == Other.Score &&
			   ArcaneType == Other.ArcaneType;
	}
};

UCLASS()
class NUMBERSANDFAMILY_API ADeck : public AActor
{
	GENERATED_BODY()
	
public:	
	ADeck();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Deck")
	TObjectPtr<UDataTable> DeckDataTable;

	TArray<FCardDataServer> Deck;

public:	
	void InitDeck();
	FCardDataServer DrawCard();
	void BackToDeck(FCardDataServer Card);	

};

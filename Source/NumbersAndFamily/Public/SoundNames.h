// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoundNames.generated.h"

UENUM(BlueprintType)
enum class ESoundRow : uint8
{
	CardDraw,
	DestructionLine,
	MoveCardAppear,
	MoveCardDisappear,
	PutCard,
	SelectCard,
	EmptyClick,
	CounterUp,
	CounterDown,
	CounterMult2,
	CounterMult3,
	StartGame,
	UINegative,
	UIPositive,
	Shuffle,
	Steal,
	Switch,
	DestructionCard,
};

inline FName ToFName(ESoundRow Row)
{
	switch (Row)
	{
	case ESoundRow::CardDraw:
		return FName("CardDraw");
	case ESoundRow::DestructionLine:
		return FName("DestructionLine");
	case ESoundRow::MoveCardAppear:
		return FName("MoveCardAppear");
	case ESoundRow::MoveCardDisappear:
		return FName("MoveCardDisappear");
	case ESoundRow::PutCard:
		return FName("PutCard");
	case ESoundRow::SelectCard:
		return FName("SelectCard");
	case ESoundRow::EmptyClick:
		return FName("EmptyClic");
	case ESoundRow::CounterUp:
		return FName("CounterUp");
	case ESoundRow::CounterDown:
		return FName("CounterDown");
	case ESoundRow::CounterMult2:
		return FName("CounterMult2");
	case ESoundRow::CounterMult3:
		return FName("CounterMult3");
	case ESoundRow::StartGame:
		return FName("StartGame");
	case ESoundRow::UINegative:
		return FName("UINegative");
	case ESoundRow::UIPositive:
		return FName("UIPositive");
	case ESoundRow::Shuffle:
		return FName("Shuffle");
	default: return FName("NONE");
	}
}

// /**
//  * 
//  */
// UCLASS()
// class NUMBERSANDFAMILY_API USoundNames : public UObject
// {
// 	GENERATED_BODY()
// 	
// };

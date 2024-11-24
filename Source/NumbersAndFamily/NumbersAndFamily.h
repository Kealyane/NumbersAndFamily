// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(Blueprintable)
enum class EPosition : uint8
{
	SERVER = 0,
	LEFT = 1,
	RIGHT = 2
};

UENUM(Blueprintable)
enum class ECardZone : uint8
{
	BOARD_LEFT,
	BOARD_RIGHT,
	POCKET,
	DECK,
};

namespace EnumHelper
{
	static FString ToString(EPosition EnumValue)
	{
		switch (EnumValue)
		{
		case EPosition::SERVER: return TEXT("SERVER");
		case EPosition::LEFT: return TEXT("LEFT");
		case EPosition::RIGHT: return TEXT("RIGHT");
		default: return TEXT("Unknown");
		}
	}

	static FString CardZoneToString(ECardZone EnumValue)
	{
		switch (EnumValue)
		{
		case ECardZone::BOARD_LEFT: return TEXT("Board Left");
		case ECardZone::BOARD_RIGHT: return TEXT("Board Right");
		case ECardZone::POCKET: return TEXT("Pocket");
		case ECardZone::DECK: return TEXT("Deck");
		default: return TEXT("Unknown");
		}
	}
}
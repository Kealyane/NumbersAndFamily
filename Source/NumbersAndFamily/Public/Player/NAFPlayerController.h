// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NumbersAndFamily/NumbersAndFamily.h"
#include "NAFPlayerController.generated.h"

class UGameWidget;
/**
 * 
 */
UCLASS()
class NUMBERSANDFAMILY_API ANAFPlayerController : public APlayerController
{
	GENERATED_BODY()

private:
	TObjectPtr<UGameWidget> GameWidget;

protected:
	UPROPERTY(EditDefaultsOnly, Category="MyPropperties|Widget")
	TSubclassOf<UGameWidget> GameWidgetType;
	UPROPERTY(EditAnywhere, Category="MyPropperties|DataTable")
	TObjectPtr<UDataTable> DeckDataTable;
	UPROPERTY(EditAnywhere, Category="MyPropperties|Card")
	TObjectPtr<UTexture2D> CardVerso;

public:
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowGameBoard();
	UFUNCTION(Client, Reliable)
	void ClientRPC_PlaceCardInPocketUI(EPosition PlayerPosition, uint8 Pos, FName CardRowName);
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowPocketCardVerso(EPosition PlayerPosition, uint8 Pos);
	UFUNCTION(Client, Reliable)
	void ClientRPC_PocketCardEmpty(EPosition PlayerPosition, uint8 Pos);
};
// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NAFPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "GameElements/Deck.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/CardWidget.h"
#include "Widgets/GameWidget.h"

void ANAFPlayerController::ClientRPC_ShowGameBoard_Implementation()
{
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow, FString(TEXT("PC : ShowGameBoard")));
	// }
	GameWidget = CreateWidget<UGameWidget>(this, GameWidgetType);
	if (GameWidget)
	{
		GameWidget->AddToViewport();
	}
	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(GameWidget->TakeWidget()); 
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	SetInputMode(InputMode); 
}

void ANAFPlayerController::ClientRPC_PlaceCardInPocketUI_Implementation(EPosition PlayerPosition, uint8 Pos,
	FName CardRowName)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow, FString(TEXT("PC : show card in UI")));
	}

	const FString ContextString(TEXT("Tower Data Context"));
	const FCardData* Data = DeckDataTable->FindRow<FCardData>(CardRowName,ContextString);
	
	if (GameWidget && Data)
	{
	 	GameWidget->GetCardWidget(PlayerPosition, Pos)->ShowCard(Data->ImageRecto);
	}

	// TODO : Sound Draw Card
	PlaySound(nullptr);
}

void ANAFPlayerController::ClientRPC_ShowPocketCardVerso_Implementation(EPosition PlayerPosition, uint8 Pos)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow, FString(TEXT("PC ShowPocketCardVerso")));
	}
	if (GameWidget)
	{
		UCardWidget* CardUW = GameWidget->GetCardWidget(PlayerPosition, Pos);
		if (CardUW)
		{
			CardUW->ShowCard(CardVerso);
		}
	}
}

void ANAFPlayerController::ClientRPC_PocketCardEmpty_Implementation(EPosition PlayerPosition, uint8 Pos)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow, FString(TEXT("PC : Pocket Card Empty")));
	}
	if (GameWidget)
	{
		GameWidget->GetCardWidget(PlayerPosition, Pos)->HideCard();
	}
}

void ANAFPlayerController::PlaySound(USoundBase* Sound)
{
	if (Sound)
	{
		UGameplayStatics::PlaySound2D(this, Sound);
	}
}

void ANAFPlayerController::UpdateActiveTurnUI(EPosition ActivePosition)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow, FString(TEXT("PC : Active Turn")));
	}
	if (GameWidget)
	{
		GameWidget->ShowActivePlayer(ActivePosition);
	}
}

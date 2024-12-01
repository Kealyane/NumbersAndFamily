// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NAFPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "GameElements/Deck.h"
#include "GameMode/NAFGameMode.h"
#include "GameMode/NAFGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Player/NAFPlayerState.h"
#include "Widgets/CardWidget.h"
#include "Widgets/GameWidget.h"

void ANAFPlayerController::ClientRPC_ShowGameBoard_Implementation()
{
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Emerald, FString(TEXT("PC : ShowGameBoard")));
	// }
	GameWidget = CreateWidget<UGameWidget>(this, GameWidgetType);
	if (GameWidget)
	{
		GameWidget->AddToViewport();
		GameWidget->OnSendHandCardSelected.AddDynamic(this, &ANAFPlayerController::GetCardTypeSelected);
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
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Emerald, FString(TEXT("PC : show card in UI")));
	// }

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
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Emerald, FString(TEXT("PC ShowPocketCardVerso")));
	// }
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
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Emerald, FString(TEXT("PC : Pocket Card Empty")));
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

void ANAFPlayerController::ServerRPC_DrawCard_Implementation()
{
	if (ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->DrawCard(GetPlayerState<ANAFPlayerState>());
	}
}

bool ANAFPlayerController::ServerRPC_DrawCard_Validate()
{
	ANAFGameState* GameState = GetWorld()->GetGameState<ANAFGameState>();
	ANAFPlayerState* NafPS = GetPlayerState<ANAFPlayerState>();
	return GameState && NafPS && GameState->ActiveId == NafPS->Id;
}

void ANAFPlayerController::ClientRPC_ActiveHand_Implementation(EPosition PlayerPosition)
{
	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Emerald,
	FString::Printf(TEXT("PC : ClientRPC_ActiveHand %s"), *EnumHelper::ToString(PlayerPosition)));
	
	GameWidget->StartHandSelection(PlayerPosition);
	GameWidget->ActiveHandHighlight(PlayerPosition);
}

void ANAFPlayerController::UpdateActiveTurnUI(EPosition ActivePosition)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Emerald,
			FString::Printf(TEXT("PC : Update UI Turn %s"), *EnumHelper::ToString(ActivePosition)));
	}
	if (GameWidget)
	{
		GameWidget->ShowActivePlayer(ActivePosition);
	}
}

void ANAFPlayerController::NotifyTurnStart()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow,
	FString(TEXT("PC : Notify Active Turn")));
	}
	bIsMyTurn = true;
	ServerRPC_DrawCard();
}

void ANAFPlayerController::EnableCardSelectionUI(EPosition PlayerId, ECardType CardType)
{
	GameWidget->ActivateHighlight(PlayerId, CardType);
}

void ANAFPlayerController::DisableCardSelectionUI()
{
	GameWidget->DeactivateHighlight();
}

void ANAFPlayerController::GetCardTypeSelected(uint8 PosInHand)
{
	ANAFPlayerState* NafPS = GetPlayerState<ANAFPlayerState>();
	ECardType CardType = NafPS->GetCardType(PosInHand);

	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow,
FString::Printf(TEXT("PC : GetCardTypeSelected %s"), *EnumCardTypeHelper::ToString(CardType)));
	
	if (CardType != ECardType::NONE)
	{
		GameWidget->ActivateHighlight(NafPS->Id, CardType);
	}
	else
	{
		GameWidget->ActiveHandHighlight(NafPS->Id);
		GameWidget->DeactivateHighlight();
	}
}

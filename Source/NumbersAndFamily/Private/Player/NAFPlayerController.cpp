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
	
	//UE_LOG(LogTemp, Warning, TEXT("PC %s: ClientRPC_ShowGameBoard_Implementation"), *EnumHelper::ToString(GetPlayerId()));
	GameWidget = CreateWidget<UGameWidget>(this, GameWidgetType);
	if (GameWidget)
	{
		GameWidget->AddToViewport();
		GameWidget->OnSendHandCardSelected.AddDynamic(this, &ANAFPlayerController::GetCardTypeSelected);
		GameWidget->OnClickBoardSlot.AddDynamic(this, &ANAFPlayerController::GetSelectedHandCard);
		GameWidget->SetIsFocusable(true);
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
	//UE_LOG(LogTemp, Warning, TEXT("PC %s : ClientRPC_PlaceCardInPocketUI_Implementation"), *EnumHelper::ToString(GetPlayerId()));
	
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
	//UE_LOG(LogTemp, Warning, TEXT("PC %s : ClientRPC_ShowPocketCardVerso_Implementation"), *EnumHelper::ToString(GetPlayerId()));
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
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Emerald, FString(TEXT("PC : Pocket Card Empty")));
	// }
	//UE_LOG(LogTemp, Warning, TEXT("PC %s : ClientRPC_PocketCardEmpty_Implementation"), *EnumHelper::ToString(GetPlayerId()));
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
		//GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Emerald, FString(TEXT("PC : DrawCard implementation")));
		//UE_LOG(LogTemp, Warning, TEXT("PC %s : ServerRPC_DrawCard_Implementation"), *EnumHelper::ToString(GetPlayerId()));
		GameMode->DrawCard(GetPlayerState<ANAFPlayerState>());
	}
}

bool ANAFPlayerController::ServerRPC_DrawCard_Validate()
{
	//GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Emerald, FString(TEXT("PC : DrawCard validation")));
	//UE_LOG(LogTemp, Warning, TEXT("PC %s : ServerRPC_DrawCard_Validate"), *EnumHelper::ToString(GetPlayerId()));
	return IsActivePlayer();
}

void ANAFPlayerController::ClientRPC_ActiveHand_Implementation(EPosition PlayerPosition)
{
	//GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Emerald,
	//FString::Printf(TEXT("PC : ClientRPC_ActiveHand %s"), *EnumHelper::ToString(PlayerPosition)));
	//UE_LOG(LogTemp, Warning, TEXT("PC %s : ClientRPC_ActiveHand_Implementation"), *EnumHelper::ToString(GetPlayerId()));
	
	GameWidget->StartHandSelection(PlayerPosition);
	GameWidget->ActiveHandHighlight(PlayerPosition);
}


void ANAFPlayerController::ServerRPC_PlaceNormalCard_Implementation(FCardDataServer Card, uint8 IndexHandCard, uint8 Line, uint8 Col)
{
	if (ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode()))
	{
// 		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Emerald,
// FString::Printf(TEXT("PC : ServerRPC_PlaceNormalCard_implementation")));
		//UE_LOG(LogTemp, Warning, TEXT("PC %s : ServerRPC_PlaceNormalCard_Implementation"), *EnumHelper::ToString(GetPlayerId()));
		GameMode->PlaceNormalCard(Card, IndexHandCard, Line, Col);
	}
}


bool ANAFPlayerController::ServerRPC_PlaceNormalCard_Validate(FCardDataServer Card, uint8 IndexHandCard, uint8 Line, uint8 Col)
{
// 	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Emerald,
// FString::Printf(TEXT("PC : ServerRPC_PlaceNormalCard_Validate")));
	UE_LOG(LogTemp, Warning, TEXT("PC %s : ServerRPC_PlaceNormalCard_Validate (%d,%d)"), *EnumHelper::ToString(GetPlayerId()), Line, Col);
	if (Card.FamilyType == EFamilyType::NONE || IndexHandCard == 0) return false;
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	return IsActivePlayer() && IsCoordInPlayerIdSide(NafPlayerState->Id, Line, Col);
}

void ANAFPlayerController::ServerRPC_EndTurn_Implementation(ANAFPlayerState* ActivePlayerState)
{
	if (!IsActivePlayer()) return;
	// GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Emerald,
	// FString::Printf(TEXT("PC : ServerRPC_EndTurn %s"), *EnumHelper::ToString(ActivePlayerState->Id)));
	//UE_LOG(LogTemp, Warning, TEXT("PC %s : ServerRPC_EndTurn_Implementation"), *EnumHelper::ToString(GetPlayerId()));
	
	if (ANAFGameMode* GameMode = Cast<ANAFGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->RemoveCardFromHand(ActivePlayerState);
		GameMode->EndTurn();
	}
}

void ANAFPlayerController::UpdateActiveTurnUI(EPosition ActivePosition)
{
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Emerald,
	// 		FString::Printf(TEXT("PC : Update UI Turn %s"), *EnumHelper::ToString(ActivePosition)));
	// }
	if (GameWidget)
	{
		//UE_LOG(LogTemp, Warning, TEXT("PC %s : UpdateActiveTurnUI"), *EnumHelper::ToString(GetPlayerId()));
		GameWidget->ShowActivePlayer(ActivePosition);
	}
}

void ANAFPlayerController::NotifyTurnStart(bool bInIsMyTurn)
{
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow,
	// FString(TEXT("PC : Notify Active Turn")));
	// }
	//UE_LOG(LogTemp, Warning, TEXT("PC %s : NotifyTurnStart"), *EnumHelper::ToString(GetPlayerId()));
	bIsMyTurn = bInIsMyTurn;
	if (bIsMyTurn) ServerRPC_DrawCard();
}

void ANAFPlayerController::EndTurn()
{
// 	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow,
// FString(TEXT("PC : EndTurn")));
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	ANAFGameState* GameState = GetWorld()->GetGameState<ANAFGameState>();
	UE_LOG(LogTemp, Warning, TEXT("PC : EndTurn"));
	if (GameState && NafPlayerState && NafPlayerState->Id == GameState->ActiveId)
	{
// 			GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow,
// FString::Printf(TEXT("PC : EndTurn player %s"), *EnumHelper::ToString(NafPS->Id)));
		//UE_LOG(LogTemp, Warning, TEXT("PC %s is active player : EndTurn"), *EnumHelper::ToString(GetPlayerId()));
		GameWidget->EndHandSelection(NafPlayerState->Id);
		GameWidget->DeactivateHandHighlight(NafPlayerState->Id);
		GameWidget->DeactivateHighlight();
		ServerRPC_EndTurn(NafPlayerState);
	}
}

void ANAFPlayerController::EnableCardSelectionUI(EPosition PlayerId, ECardType CardType)
{
	GameWidget->ActivateHighlight(PlayerId, CardType);
}

// void ANAFPlayerController::DisableCardSelectionUI()
// {
// 	GameWidget->DeactivateHighlight();
// }

void ANAFPlayerController::GetCardTypeSelected(uint8 PosInHand)
{
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	if (!IsActivePlayer() && !NafPlayerState) return;
	
	ECardType CardType = NafPlayerState->GetCardType(PosInHand);
		
	// 	GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow,
	// FString::Printf(TEXT("PC : GetCardTypeSelected %s"), *EnumCardTypeHelper::ToString(CardType)));
	UE_LOG(LogTemp, Warning, TEXT("PC %s : GetCardTypeSelected"), *EnumHelper::ToString(GetPlayerId()));

	if (CardType != ECardType::NONE)
	{
		GameWidget->ActivateHighlight(NafPlayerState->Id, CardType);
	}
	else
	{
		GameWidget->ActiveHandHighlight(NafPlayerState->Id);
		GameWidget->DeactivateHighlight();
	}
	
}

void ANAFPlayerController::GetSelectedHandCard(uint8 Line, uint8 Col)
{
	if (!IsActivePlayer()) return;
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	UE_LOG(LogTemp, Warning, TEXT("PC %s : GetSelectedHandCard"), *EnumHelper::ToString(GetPlayerId()));
	if (NafPlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("PC %s : GetSelectedHandCard (%d,%d)"), *EnumHelper::ToString(GetPlayerId()), Line, Col);
		ServerRPC_PlaceNormalCard(NafPlayerState->GetSelectedCard(), NafPlayerState->GetIndexSelected(), Line, Col);
	}
}

void ANAFPlayerController::UpdateBoardCard(bool bAfterPlayerAction, const TArray<FName>& InBoardTableRow)
{
	// GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow,
	// FString::Printf(TEXT("PC : UpdateBoardCard")));
	UE_LOG(LogTemp, Warning, TEXT("PC %s : UpdateBoardCard"), *EnumHelper::ToString(GetPlayerId()));

	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	if (!NafPlayerState) return;

	NbCardInBoard = 0;
	NBCardInOpponentBoard = 0;
	
	const FString ContextString(TEXT("Card Data Context"));
	for (int i = 0; i < InBoardTableRow.Num(); i++)
	{
		int row = i / 6;
		int col = i % 6;
		if (row >= 0 && row < 3 && col >= 0 && col < 6)
		{
			if (UCardWidget* CardSlot = GameWidget->BoardSlots[row][col])
			{
				if (InBoardTableRow[i].IsNone())
				{
					CardSlot->HideCard();
				}
				else
				{
					const FCardData* Data = DeckDataTable->FindRow<FCardData>(InBoardTableRow[i],ContextString);
					if (GameWidget && Data)
					{
						UE_LOG(LogTemp, Warning, TEXT("PC : UpdateBoardCard = show card"));
						CardSlot->ShowCard(Data->ImageRecto);
						
						if (IsCoordInPlayerIdSide(NafPlayerState->Id, row, col))
						{
							NbCardInBoard++; 
						}
						else
						{
							NBCardInOpponentBoard++;
						}
					}
				}
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow,
FString::Printf(TEXT("PC : UpdateBoardCard no card slot")));
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("PC %s : UpdateBoardCard my card = %d - opponent = %d"),
		*EnumHelper::ToString(NafPlayerState->Id), NbCardInBoard, NBCardInOpponentBoard);
	if (bAfterPlayerAction)
	{
		EndTurn();
	}
}

bool ANAFPlayerController::IsCoordInPlayerIdSide(EPosition PlayerId, uint8 Line, uint8 Col)
{
	if (PlayerId == EPosition::LEFT)
	{
		return Line < 3 && Col < 3;
	}
	return Line < 3 && Col >= 3 && Col < 6;
}

bool ANAFPlayerController::IsActivePlayer() const
{
	ANAFGameState* GameState = GetWorld()->GetGameState<ANAFGameState>();
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	// if (GameState && NafPlayerState && NafPlayerState->Id == GameState->ActiveId)
	// {
	// 	UE_LOG(LogTemp, Warning, TEXT("PC : is active player : activeID = %s, playerID = %s"),
	// 		*EnumHelper::ToString(GameState->ActiveId),
	// 		*EnumHelper::ToString(NafPlayerState->Id));
	// }
	return GameState && NafPlayerState && NafPlayerState->Id == GameState->ActiveId;
}

EPosition ANAFPlayerController::GetPlayerId() const
{
	ANAFPlayerState* NafPlayerState = GetPlayerState<ANAFPlayerState>();
	if (!NafPlayerState) return EPosition::SERVER;
	return NafPlayerState->Id;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NAFGameMode.h"

#include "GameElements/Board.h"
#include "GameFramework/GameState.h"
#include "GameMode/NAFGameState.h"
#include "Player/NAFPlayerController.h"
#include "Player/NAFPlayerState.h"
#include "GameElements/Deck.h"
#include "MultiplayerSessionsSubsystem.h"

void ANAFGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState->PlayerArray.Num();

	if (ANAFPlayerController* TPC = Cast<ANAFPlayerController>(NewPlayer))
	{
		if (ANAFPlayerState* PS = Cast<ANAFPlayerState>(TPC->PlayerState))
		{
			
			FString CurrentPlayerName;
			if (!IS_EDITOR)
			{
				if (UGameInstance* GameInstance =  GetGameInstance())
				{
					if (UMultiplayerSessionsSubsystem* MultiplayerSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>())
					{
						CurrentPlayerName = MultiplayerSubsystem->GetSteamNickname(NewPlayer);
					}
				}
			}
			if (NumberOfPlayers == 1)
			{
				PS->Id = EPosition::LEFT;
				if (CurrentPlayerName.IsEmpty() || CurrentPlayerName == TEXT("Unknown"))
				{
					PS->PlayerAlias = FName("Host");
				}
				else
				{
					P1Name = FName(*CurrentPlayerName);
					PS->PlayerAlias = FName(*CurrentPlayerName);
				}
			}
			else if (NumberOfPlayers == 2)
			{
				PS->Id = EPosition::RIGHT;
				if (CurrentPlayerName.IsEmpty() || CurrentPlayerName == TEXT("Unknown"))
				{
					PS->PlayerAlias = FName("Guest");
				}
				else
				{
					P2Name = FName(*CurrentPlayerName);
					PS->PlayerAlias = FName(*CurrentPlayerName);
				}
			}
		}
	}
	
	
	if (NumberOfPlayers == 2)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(WaitHandle, this, &ANAFGameMode::LaunchGame, 1.f, false);
		}
	}
}

void ANAFGameMode::LaunchGame()
{
	bIsGameOver = false;
	Winner = EPosition::SERVER;

	UWorld* World = GetWorld();
	
	if (World)
	{
		World->GetTimerManager().ClearTimer(WaitHandle);
	}

	NafGameState = GetGameState<ANAFGameState>();
	
	// Link to Level Blueprint to remove lobby widget
	if (NafGameState)
	{
		NafGameState->OnLeaveLobby.Broadcast();
	}

	// Show UI
	for (FConstPlayerControllerIterator PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; ++PCIterator)
	{
		if (ANAFPlayerController* NafPlayerController = Cast<ANAFPlayerController>(*PCIterator))
		{
			NafPlayerController->ClientRPC_ShowGameBoard();
			NafGameState->SetPlayerNames();
		}
	}

	// Initialize Deck
	if (World)
	{
		Deck = World->SpawnActor<ADeck>(DeckClassType);
		if (!Deck->InitDeck()) return;
		Board = World->SpawnActor<ABoard>(BoardClassType);
		Board->InitBoard();
		Board->InitParamDeck(Deck);
		NafGameState->InitBoardRow();
	

		NafGameState->MultiRPC_PlaySoundForBoth(ESoundRow::StartGame);
		NafGameState->MultiRPC_PlayMusicForBoth();

		DelayCounter += 1.5f;
		World->GetTimerManager().SetTimer(ShuffleHandle,
			[this]()
			{
				NafGameState->MultiRPC_PlaySoundForBoth(ESoundRow::Shuffle);
			},
			DelayCounter,
			false);
		
		DelayCounter += 3.f;
		World->GetTimerManager().SetTimer(InitCardHandle,this,&ANAFGameMode::GiveOneCardToBothPlayer, DelayCounter, false);
		
		DelayCounter += 4.5f;
		World->GetTimerManager().SetTimer(SwitchPlayerHandle,this,&ANAFGameMode::InitializeCurrentPlayer, DelayCounter, false);
	}
	
}

void ANAFGameMode::EndGame()
{
	if (NafGameState)
	{
		NafGameState->MultiRPC_EndGame(Winner);
	}
}

void ANAFGameMode::DrawCard(ANAFPlayerState* ActivePlayerState)
{
	if (bIsGameOver)
	{
		EndGame();
		return;
	}

	//NafGameState->MultiRPC_PlaySoundForBoth(ESoundRow::Shuffle);
	FCardDataServer Card = Deck->DrawCard();
	if (Card.RowName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode : Can't draw card, empty"));
		return;
	}
	
	GetWorld()->GetTimerManager().SetTimer(ShuffleHandle,
	[this, ActivePlayerState, Card]()
	{
		NafGameState->MultiRPC_PlaySoundForBoth(ESoundRow::CardDraw);
		ActivePlayerState->StoreCardInHand(Card);
		TArray<bool> HandCurrent = ActivePlayerState->HandStatus();
		ANAFPlayerState* OpponentPS = NafGameState->GetOpponentPlayerState(ActivePlayerState->Id);
		OpponentPS->UpdateHandUI(ActivePlayerState->Id,HandCurrent);
		FTimerHandle WaitHandle;
		GetWorld()->GetTimerManager().SetTimer(WaitHandle,
			[this, ActivePlayerState]()
			{
				ActivePlayerState->ActiveHandChoice(ActivePlayerState->Id);
			}, 1.5f, false);
	},
	0.5f,
	false);
}

void ANAFGameMode::RemoveCardFromHand(ANAFPlayerState* ActivePlayerState)
{
	ActivePlayerState->RemoveCardInHand(IndexHandCardPlayed);
	TArray<bool> HandCurrent = ActivePlayerState->HandStatus();
	ANAFPlayerState* OpponentPS = NafGameState->GetOpponentPlayerState(ActivePlayerState->Id);
	OpponentPS->UpdateHandUI(ActivePlayerState->Id,HandCurrent);
}

void ANAFGameMode::PlaceNormalCard(FCardDataServer Card, uint8 IndexHandCard, uint8 Line, uint8 Col)
{
	IndexHandCardPlayed = IndexHandCard;
	FTimerHandle PlayAnimPutCardHandle;
	GetWorld()->GetTimerManager().SetTimer(PlayAnimPutCardHandle, [this, Card, Line, Col]()
	{
		Board->PlaceNormalCard(Card, Line, Col);
	}, 0.5f, false);
	
	NafGameState->MultiRPC_ShowCard(Card, Line, Col);
	NafGameState->MultiRPC_PlaySoundForBoth(ESoundRow::PutCard);
	NafGameState->MultiRPC_PutCard(Line,Col);
}

void ANAFGameMode::EndTurn()
{
	NafGameState->SwitchPlayerTurn();
}

bool ANAFGameMode::IsCoordOccupiedInBoard(uint8 Line, uint8 Col)
{
	return Board->IsCoordOccupied(Line, Col);
}

void ANAFGameMode::SwitchCardsInBoard(uint8 IndexHandCard, uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col)
{
	IndexHandCardPlayed = IndexHandCard;
	Board->SwitchCard(Card1Line, Card1Col, Card2Line, Card2Col);
}

void ANAFGameMode::StealCardInBoard(uint8 IndexHandCard, uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col)
{
	IndexHandCardPlayed = IndexHandCard;
	Board->StealCard(Card1Line, Card1Col, Card2Line, Card2Col);
}

FName ANAFGameMode::GetCardDataRowName(uint8 Line, uint8 Col)
{
	return Board->GetCardDataRowName(Line, Col);
}

void ANAFGameMode::CopyCardInBoard(uint8 IndexHandCard, uint8 Card1Line, uint8 Card1Col, uint8 Card2Line, uint8 Card2Col, FCardDataServer Card)
{
	IndexHandCardPlayed = IndexHandCard;
	Board->CopyCard(Card1Line, Card1Col, Card2Line, Card2Col, Card);
}

FName ANAFGameMode::GetRowNameFromDataServer(FCardDataServer Card)
{
	return Deck->GetRowNameFromDataServer(Card);
}

void ANAFGameMode::SetGameOverInfos(EPosition WinnerId)
{
	bIsGameOver = true;
	Winner = WinnerId;
}


void ANAFGameMode::GiveOneCardToBothPlayer()
{
	if (NafGameState && NafGameState->PlayerArray.Num() == 2)
	{
		APlayerState* PlayerState1 = NafGameState->PlayerArray[0];
		APlayerState* PlayerState2 = NafGameState->PlayerArray[1];
		if (PlayerState1 && PlayerState2)
		{
			ANAFPlayerState* NafPS1 = Cast<ANAFPlayerState>(PlayerState1);
			ANAFPlayerState* NafPS2 = Cast<ANAFPlayerState>(PlayerState2);
			if (NafPS1 && NafPS2)
			{
				FCardDataServer Card = Deck->DrawCard();
				NafPS1->StoreCardInHand(Card);
				
				FTimerHandle GiveCard1AnimHandle;

				GetWorld()->GetTimerManager().SetTimer(GiveCard1AnimHandle,
					[this, NafPS1, NafPS2]()
					{
						TArray<bool> HandCurrent = NafPS1->HandStatus();
						NafPS2->UpdateHandUI(NafPS1->Id,HandCurrent);

						FCardDataServer Card2 = Deck->DrawCard();
						Deck->bIsStartGame = false;
						NafPS2->StoreCardInHand(Card2);
						HandCurrent = NafPS2->HandStatus();
						NafPS1->UpdateHandUI(NafPS2->Id,HandCurrent);
						
					}, 2.1f, false);
			}
		}
	}
}

void ANAFGameMode::InitializeCurrentPlayer()
{
	// Initialize Active Player
	bool bIsPlayerLeft = FMath::RandBool();
	EPosition ActivePosition = bIsPlayerLeft ? EPosition::LEFT : EPosition::RIGHT;
	
	NafGameState->SetActivePlayer(ActivePosition);
}

void ANAFGameMode::NewGame()
{
	bIsGameOver = false;
	Winner = EPosition::SERVER;

	if (ANAFPlayerState* P1 = NafGameState->GetNafPlayerState(EPosition::LEFT))
	{
		P1->Card1 = FCardDataServer();
		P1->Card2 = FCardDataServer();
	}
	if (ANAFPlayerState* P2 = NafGameState->GetNafPlayerState(EPosition::LEFT))
	{
		P2->Card1 = FCardDataServer();
		P2->Card2 = FCardDataServer();
	}
	
	// Initialize Deck
	if (UWorld* World = GetWorld())
	{
		Deck->ResetDeck();
		Board->InitBoard();
		Board->InitParamDeck(Deck);
		NafGameState->InitBoardRow();
		NafGameState->MultiRPC_UpdateScores(0,0,0,0,
			0,0,0,0);
		
		// Show UI
		for (FConstPlayerControllerIterator PCIterator = GetWorld()->GetPlayerControllerIterator(); PCIterator; ++PCIterator)
		{
			if (ANAFPlayerController* NafPlayerController = Cast<ANAFPlayerController>(*PCIterator))
			{
				// REMOVE ENDGAME WIDGET
				NafPlayerController->ClientRPC_ResetShowGameBoard();
			}
		}
		NafGameState->MultiRPC_PlaySoundForBoth(ESoundRow::StartGame);

		DelayCounter += 0.5f;
		World->GetTimerManager().SetTimer(ShuffleHandle,
			[this]()
			{
				NafGameState->MultiRPC_PlaySoundForBoth(ESoundRow::Shuffle);
			},
			DelayCounter,
			false);
		
		DelayCounter += 3.f;
		World->GetTimerManager().SetTimer(InitCardHandle,this,&ANAFGameMode::GiveOneCardToBothPlayer, DelayCounter, false);
		
		DelayCounter += 4.5f;
		World->GetTimerManager().SetTimer(SwitchPlayerHandle,this,&ANAFGameMode::InitializeCurrentPlayer, DelayCounter, false);
	}
}





// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 2, FString TypeMatch = FString(TEXT("Duel")),
		FString LobbyPath = FString(TEXT("/Game/ThirdPerson/Maps/Lobby")));
	UFUNCTION(BlueprintCallable)
	void MenuCancelSetup();

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	//
	// Callbacks for the custom delegates on the MultiplayerSessionsSubsystem
	//
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);

	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);

	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);

	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	
private:
	UPROPERTY(meta = (BindWidget))
	class UButton* HostButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* JoinButton;

	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void JoinButtonClicked();

	// the subsystem designed to handle all online session functionality
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	int32 NumPublicConnections{2};
	FString MatchType {TEXT("Duel")};
	bool bAllowJoinInProgress {false};
	FString PathToLobby {TEXT("")};
};

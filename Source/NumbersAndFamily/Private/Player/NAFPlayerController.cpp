// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/NAFPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Widgets/GameWidget.h"

void ANAFPlayerController::ClientRPC_ShowGameBoard_Implementation()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 90.f, FColor::Yellow, FString(TEXT("PC : ShowGameBoard")));
	}
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

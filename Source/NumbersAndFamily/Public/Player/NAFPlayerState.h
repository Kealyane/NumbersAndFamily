// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NumbersAndFamily/NumbersAndFamily.h"
#include "NAFPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class NUMBERSANDFAMILY_API ANAFPlayerState : public APlayerState
{
	GENERATED_BODY()

	ANAFPlayerState();

public:
	UPROPERTY(ReplicatedUsing=OnRep_Id)
	EPosition Id = EPosition::SERVER;

	
	UFUNCTION()
	void OnRep_Id();

protected:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SoundNames.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundCue.h"
#include "AudioContainer.generated.h"

UCLASS()
class NUMBERSANDFAMILY_API AAudioContainer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAudioContainer();

	void PlayAudio(ESoundRow SoundToPlay);
	int GetAllSoundsSize() { return AllSounds.Num();}

protected:
	UPROPERTY(EditDefaultsOnly, Category="Sound")
	TMap<ESoundRow,USoundCue*> AllSounds;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/AudioContainer.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AAudioContainer::AAudioContainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AAudioContainer::PlayAudio(ESoundRow SoundToPlay)
{
	if (AllSounds.Contains(SoundToPlay))
	{
		if (USoundCue* SoundCue = AllSounds[SoundToPlay])
		{
			UGameplayStatics::PlaySound2D(GetWorld(), SoundCue);
		}
	}
}



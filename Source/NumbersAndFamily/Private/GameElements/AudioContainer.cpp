// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/AudioContainer.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAudioContainer::AAudioContainer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioMusic"));
	AudioComponent->bAutoActivate = false; 
	AudioComponent->bIsUISound = true;   
	AudioComponent->bAllowSpatialization = false; 
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

void AAudioContainer::PlayMusic()
{
	OnPlayMusic.Broadcast();
}



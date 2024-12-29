// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/NAFSoundManager.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void UNAFSoundManager::PlaySoundByRowName(FName RowName)
{
	FSoundData SoundData;
	if (GetSoundDataByRowName(RowName, SoundData))
	{
		if (SoundData.SoundCue.IsValid())
		{
			USoundCue* Sound = SoundData.SoundCue.Get();
			UGameplayStatics::PlaySound2D(GetWorld(), Sound);
		}
	}
}

void UNAFSoundManager::PlaySoundAndWait(FName RowName, FLatentActionInfo LatentInfo)
{
	FSoundData SoundData;
	if (GetSoundDataByRowName(RowName, SoundData))
	{
		if (SoundData.SoundCue.IsValid())
		{
			USoundCue* Sound = SoundData.SoundCue.Get();
			if (Sound)
			{
				UAudioComponent* AudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), Sound);
				if (AudioComponent)
				{
					UKismetSystemLibrary::Delay(GetWorld(), SoundData.DelayTime, LatentInfo);
				}
			}
		}
	}
}

bool UNAFSoundManager::GetSoundDataByRowName(FName RowName, FSoundData& OutSoundData)
{
	if (!DTSound) return false;

	FSoundData* FoundSound = DTSound->FindRow<FSoundData>(RowName, TEXT("Search Sound"));
	if (FoundSound)
	{
		OutSoundData = *FoundSound;
		return true;
	}
	return false;
}

void UNAFSoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	FString AssetPath = TEXT("/Game/Datas/DT_Sounds.uasset");
	UDataTable* LoadTable = LoadObject<UDataTable>(nullptr, *AssetPath);
	if (LoadTable)
	{
		DTSound = LoadTable;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Sound Data Table"));
	}
}


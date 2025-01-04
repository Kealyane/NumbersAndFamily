// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NAFSoundManager.generated.h"

class USoundCue;

USTRUCT(BlueprintType)
struct FSoundData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* SoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayTime;

	FSoundData() : DelayTime(0.0f) {}
};

/**
 * 
 */
UCLASS()
class NUMBERSANDFAMILY_API UNAFSoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

// public:
// 	UFUNCTION(BlueprintCallable)
// 	void PlaySoundByRowName(FName RowName);
//
// 	UFUNCTION(BlueprintCallable)
// 	void PlaySoundAndWait(FName RowName, FLatentActionInfo LatentInfo);
//
// protected:
// 	UPROPERTY(EditDefaultsOnly, Category="Sounds")
// 	UDataTable* DTSound;
// 	bool GetSoundDataByRowName(FName RowName, FSoundData& OutSoundData);
//
// 	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};

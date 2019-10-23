// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "BSSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class TEAM2_PROJECT2_API UBSSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = Basic)
	FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, Category = Basic)
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Level)
	FString CurrentLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Level)
	FString NextLevel;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Level)
	float MusicVolume = 1.0f;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Level)
	float SFXVolume = 1.0f;

	UBSSaveGame();
};

// Fill out your copyright notice in the Description page of Project Settings.

#include "Team2_Project2GameModeBase.h"
#include "Components/BSElementComponentSystem.h"
#include <EngineUtils.h>
#include "Core/BSAIAgent.h"
#include <Kismet/GameplayStatics.h>
#include "Core/BSSaveGame.h"

void ATeam2_Project2GameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Won = false;
	DeadOrcCounter = 0;
	NumberOfOrcs = 0;
	CurrentScore = 0;

	for (TActorIterator<ABSAIAgent> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		NumberOfOrcs++;
		ActorItr->OnDeath.AddDynamic(this, &ATeam2_Project2GameModeBase::HandleOrcDeath);
	}

	deathCounters.Empty();

	FString currentLevel = UGameplayStatics::GetCurrentLevelName(GetWorld());
	bool foundLevel = false;
	const int32 numberOfLevels = LevelsInOrder.Num();
	int32 currentLevelIndex = LevelsInOrder.Find(currentLevel);
	UE_LOG(LogTemp, Log, TEXT("Current level index: %d"), currentLevelIndex);

	for (FDeathsPerLevel& deathsPerLevel : PossibleDeathsPerLevel)
	{
		if(!deathsPerLevel.LevelName.Equals(currentLevel))
			continue;

		for (EDeathType& deathType : deathsPerLevel.PossibleDeaths)
		{
			deathCounters.Add(deathType, 0);
		}
		foundLevel = true;
	}

	if (!foundLevel)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current level does not exsist in possible deaths per level data structure, in game mode"));
	}

	FString nextLevel = TEXT("MainMenu");
	if (currentLevelIndex >= 0 && currentLevelIndex < numberOfLevels - 1)
		nextLevel = LevelsInOrder[currentLevelIndex + 1];

	UBSSaveGame* SaveGameInstance = Cast<UBSSaveGame>(UGameplayStatics::CreateSaveGameObject(UBSSaveGame::StaticClass()));
	SaveGameInstance->CurrentLevel = currentLevel;
	SaveGameInstance->NextLevel = nextLevel;
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);

	OnGameStart(NumberOfOrcs);
}
ABSElementComponentSystem* ATeam2_Project2GameModeBase::GetElementComponentSystem()
{
	if(componentSystemInstance  == nullptr)
		componentSystemInstance = Cast<ABSElementComponentSystem>(GetWorld()->SpawnActor(ElementComponentSystem));

	return componentSystemInstance;
}
void ATeam2_Project2GameModeBase::HandleVictory()
{
	if (Won)
		return;
	Won = true;

	AddScore(BaseSurviveScore * GetRemainingOrcs());

	OnGameOver(deathCounters, true);
}

int32 ATeam2_Project2GameModeBase::GetRemainingOrcs()
{
	return NumberOfOrcs - DeadOrcCounter;
}


const TMap<EDeathType, int32>& ATeam2_Project2GameModeBase::GetSortedOrcDeaths()
{
	deathCounters.ValueSort([](int32 A, int32 B) { return A >= B; });
	return deathCounters;
}

void ATeam2_Project2GameModeBase::HandleOrcDeath(EDeathType death)
{
	if (Won)
		return;
	
	if (deathCounters.Contains(death))
		deathCounters[death]++;
	else
		deathCounters.Add(death, 1);
	DeadOrcCounter++;
	OnOrcDeath(NumberOfOrcs - DeadOrcCounter);

	float score = BaseDeathScore;
	if (ScoreMultipliers.Contains(death))
		score *= ScoreMultipliers[death];

	AddScore(score);

	if (NumberOfOrcs == DeadOrcCounter)
	{
		OnGameOver(deathCounters, false);
	}
}
void ATeam2_Project2GameModeBase::AddScore(float scoreToAdd)
{
	CurrentScore += scoreToAdd;
	OnScoreUpdate(FMath::RoundToInt(CurrentScore));
}

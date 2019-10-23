#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Team2_Project2GameModeBase.generated.h"

UENUM(BlueprintType)
enum class EDeathType : uint8 
{
	Default		UMETA(DisplayName = "Default"),
	Burned		UMETA(DisplayName = "Burned"),
	Exploded	UMETA(DisplayName = "Exploded"),
	Minced		UMETA(DisplayName = "Minced"),
	Crushed		UMETA(DisplayName = "Crushed"),
	Shot		UMETA(DisplayName = "Shot"),
	Spiked		UMETA(DisplayName = "Spiked"),
	Drowned		UMETA(DisplayName = "Drowned"),
	Fall		UMETA(DisplayName = "Fall"),
	Decapitated	UMETA(DisplayName = "Decapitated"),
	Splatted	UMETA(DisplayName = "Splatted")
};

USTRUCT(BlueprintType)
struct TEAM2_PROJECT2_API FDeathsPerLevel
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString LevelName;
	UPROPERTY(EditAnywhere)
	TArray<EDeathType> PossibleDeaths;
};


UCLASS()
class TEAM2_PROJECT2_API ATeam2_Project2GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ABSElementComponentSystem> ElementComponentSystem;
	UPROPERTY(BlueprintReadWrite)
	bool Won;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FString> LevelsInOrder;
	UPROPERTY(EditDefaultsOnly)
	TArray<FDeathsPerLevel> PossibleDeathsPerLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Score)
	float CurrentScore;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Score)
	int32 BaseDeathScore;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Score)
	int32 BaseSurviveScore;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Score)
	TMap<EDeathType, float> ScoreMultipliers;
private:
	int32 NumberOfOrcs;
	int32 DeadOrcCounter;
	class ABSElementComponentSystem* componentSystemInstance;
	TMap<EDeathType, int32> deathCounters;
public:	
	void BeginPlay() override;
	ABSElementComponentSystem* GetElementComponentSystem();
	UFUNCTION(BlueprintImplementableEvent)
	void OnGameStart(int32 numberOfOrcs);
	UFUNCTION(BlueprintImplementableEvent)
	void OnGameOver(const TMap<EDeathType, int32>& deathTypes, bool victory);
	UFUNCTION(BlueprintImplementableEvent)
	void OnOrcDeath(int32 orcsRemaining);
	UFUNCTION(BlueprintCallable)
	void HandleVictory();
	UFUNCTION(BlueprintCallable)
	int32 GetRemainingOrcs();
	UFUNCTION(BlueprintImplementableEvent)
	void OnScoreUpdate(int32 score);
	UFUNCTION(BlueprintCallable)
	const TMap<EDeathType, int32>& GetSortedOrcDeaths();
private:
	UFUNCTION()
	void HandleOrcDeath(EDeathType death);
	void AddScore(float scoreToAdd);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <Camera/CameraComponent.h>
#include <Engine/World.h>
#include "BSCameraShaker.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM2_PROJECT2_API UBSCameraShaker : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBSCameraShaker();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float CurrentIntensity = 0;
	UPROPERTY(EditAnywhere)
	float MaxDuration = 1;
	UPROPERTY(EditAnywhere)
	FVector PerlinSpeed;
	UPROPERTY(EditAnywhere)
	FVector MaxShake;
	UPROPERTY(EditAnywhere)
	float MaxRoll;
	UPROPERTY(EditAnywhere)
	float RollSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCameraComponent* CameraRef;

	UWorld* World;


public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = Camera ,meta = (ToolTip = "0.0 to 1.0 value affection"))
	void AddCameraShake(float x);
		
};

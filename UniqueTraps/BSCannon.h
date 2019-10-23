// Fill out your copyright notice in the Description page of Project Settings.
// Author: Simon
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Core/BSAIAgent.h"
#include <Engine/TargetPoint.h>
#include "BSCannon.generated.h"

UCLASS()
class TEAM2_PROJECT2_API ABSCannon : public AActor
{
	GENERATED_BODY()
	
public:	
	//Temp
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ABSAIAgent* orc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATargetPoint* Target;

protected:
	ABSAIAgent* AmmoSlot;

public:	
	ABSCannon();
	UFUNCTION(BlueprintCallable, Category = "Cannon")
	bool LoadProjectile(ABSAIAgent* OrcToLoad, FVector Location);
	UFUNCTION(BlueprintCallable, Category = "Cannon")
	bool FireProjectile();
	UFUNCTION(BlueprintCallable, Category = "Cannon")
	bool RotateCannon();


};

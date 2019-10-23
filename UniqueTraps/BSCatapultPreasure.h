// Fill out your copyright notice in the Description page of Project Settings.
// Author: Simon
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Engine/TargetPoint.h>
#include <Components/ShapeComponent.h>
#include "Core/BSAIAgent.h"
#include "BSCatapultPreasure.generated.h"

UCLASS()
class TEAM2_PROJECT2_API ABSCatapultPreasure : public AActor
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProjectileFireDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FProjectileOnDelayStartDelegate);

public:	
	ABSCatapultPreasure();
	virtual void BeginPlay() override;
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFrameSweep, const FHitResult& SweepResults);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);
	void Launch();

protected:

public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UShapeComponent* Collider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATargetPoint* Target;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Active = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Time = 5.0f;
	UPROPERTY(BlueprintAssignable, Category = BSCatapult)
	FProjectileFireDelegate OnProjectileFire;
	UPROPERTY(BlueprintAssignable, Category = BSCatapult)
	FProjectileOnDelayStartDelegate OnDelayStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float orcSpacing = 25.0f;

protected:
	FTimerHandle untilNextLaunch;
	FTimerHandle delayedDisable;
	FTimerHandle reactive;

	UPROPERTY(VisibleAnywhere)
	bool loaded = false;

	TArray<ABSAIAgent*> agentList;

};

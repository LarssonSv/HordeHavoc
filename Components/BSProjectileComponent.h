// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSProjectileComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM2_PROJECT2_API UBSProjectileComponent : public UActorComponent
{
	GENERATED_BODY()

		DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FProjectileFireDelegate, FRotator, Direction);
		DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FProjectileHitDelegate, FHitResult, Hit, FVector, VelocityOnHit);

public:
	UPROPERTY(EditDefaultsOnly)
	float Gravity = 10000;
	UPROPERTY(EditDefaultsOnly)
	float TimeToImpact = 2.0f;
	UPROPERTY(BlueprintAssignable)
	FProjectileHitDelegate OnProjectileHit;
	UPROPERTY(BlueprintAssignable)
	FProjectileFireDelegate OnProjectileFire;
	UPROPERTY(VisibleAnywhere)
	bool Activated = false;
	UPROPERTY(EditAnywhere)
	float Accuracy = 100;
	UPROPERTY(BlueprintReadOnly)
	FVector Velocity;
private:
	FVector StartLocation;
	FVector TargetLocation;
	FRotator TrajectoryRotation;
	float AngleInRads;
	float InitialVelocity;
	
public:	
	UBSProjectileComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	

	UFUNCTION(BlueprintCallable)
	void SetTarget(const FVector& worldLocation);
};

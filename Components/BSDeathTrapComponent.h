// Fill out your copyright notice in the Description page of Project Settings.
//Author: Simon
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include <Components/SphereComponent.h>
#include <Components/SceneComponent.h>
#include "Team2_Project2GameModeBase.h"
#include "BSDeathTrapComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM2_PROJECT2_API UBSDeathTrapComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

		DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnKillDelegate, AActor*, DeadOrc, FVector, HitPoint);


public:	
	UBSDeathTrapComponent();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (ToolTip = "Current Orcs killed"))
	int32 deathCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Set trap on/off"))
	bool trapActive = true;
	UPROPERTY(BlueprintAssignable)
	FOnKillDelegate OnKill;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Set collider in construction!"))
	TArray<UShapeComponent*> DeathColliders;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDeathType DeathType = EDeathType::Default;

protected:
	

public:	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Gets Orcs killed"))
	int32 GetDeathCount() const;


protected:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFrameSweep, const FHitResult& SweepResults);
		
};

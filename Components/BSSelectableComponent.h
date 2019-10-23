// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSSelectableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM2_PROJECT2_API UBSSelectableComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelectionDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRightClickDelegate, FVector, Location);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool CanBeSelected = true;

	UPROPERTY(EditDefaultsOnly, Category = Temp)
	float Speed;
	UPROPERTY(EditDefaultsOnly, Category = Temp)
	float StopDistance;

	UPROPERTY(BlueprintAssignable)
	FOnSelectionDelegate OnSelect;
	UPROPERTY(BlueprintAssignable)
	FOnSelectionDelegate OnPreSelect;

	UPROPERTY(BlueprintAssignable)
	FOnSelectionDelegate OnDeselect;
	UPROPERTY(BlueprintAssignable)
	FOnRightClickDelegate OnMoveCommand;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsSelected = false;

private:
	FVector TargetLocation;
	bool canBeSelectedValue;

public:	
	UBSSelectableComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Deactivate();
	void ReActivate();

	UFUNCTION(BlueprintCallable)
	void TempMove(FVector location);
};

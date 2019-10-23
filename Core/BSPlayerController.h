// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TEAM2_PROJECT2_API ABSPlayerController : public APlayerController
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectionUpdateDelegate);

public:
	UPROPERTY(BlueprintAssignable)
	FSelectionUpdateDelegate OnSelectionUpdate;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> MoveCommandIndicator = nullptr;
private:
	class ABSHUD* Hud;
	TArray<class UBSSelectableComponent*> Selectables;
	bool SelectionModifierPressed;
	bool ShowMovementIndicator;
	bool RightClickDown;
	const float MOVE_COMMAND_COOLDOWN = 0.25f;
	float TimeOfLastMoveCommand;
	bool Paused;
	float angleStep = 40.f;
	float currentDistance = 200.f;
	float currentAngle = 0.f;
	const float DISTANCE_STEP = 200.f;
	const float AngleFactor = 0.73f;
public:
	ABSPlayerController();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupInputComponent() override;	
	void Deselect();
	bool GetSelectedCenter(FVector& outCenter) const;
	UFUNCTION(BlueprintImplementableEvent)
	void OnPause(bool paused);
	UFUNCTION(BlueprintCallable)
	void SelectNearestOrc();
	UFUNCTION(BlueprintCallable)
	void IgnoreNextHudInput();

private:
	void HandleLeftClickPressed();
	void HandleLeftDoubleClickPressed();
	void HandleLeftClickReleased();
	void HandleRightClickPressed();
	void HandleRightClickReleased();
	void HandleSelectionModifierPressed();
	void HandleSelectionModifierReleased();
	void HandleRestartPressed();
	void HandleSelectClosestIdlePressed();
	UFUNCTION()
	void UpdateSelection(TArray<class UBSSelectableComponent*> selectables);
	void HandleSelectAllPressed();
	void SetMoveLocation();

};

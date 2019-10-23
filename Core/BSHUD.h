// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BSHUD.generated.h"

/**
 * 
 */
UCLASS()
class TEAM2_PROJECT2_API ABSHUD : public AHUD
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectionDelegate, TArray<class UBSSelectableComponent*>, selectables);

public:
	UPROPERTY(EditDefaultsOnly, Category = BoxSelect)
	FLinearColor BoxColor;
	UPROPERTY(EditDefaultsOnly, Category = BoxSelect)
	float MinimumMovementToReselect = 5.0f;

	UPROPERTY(BlueprintAssignable)
	FOnSelectionDelegate OnSelectionUpdate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Enabled = true;
private:
	bool MouseDown;
	bool DoSelectionUpdate;
	bool SendSelectionUpdate;
	bool SelectAllVisible;
	FVector2D StartMousePosition;
	FVector2D LatestMousePosition;
	bool justDoubleClicked;
	float timeOfDoubleClick;
	const float DOUBLE_CLICK_COOLDOWN = 0.1f;
	bool ignoreNext;
	bool firstInputGiven;
	//TODO: should give selectables an id, and make these into hashmaps
	TArray<UBSSelectableComponent*> CurrentSelectables;
	TArray<UBSSelectableComponent*> DeselectedSelectables;
public:
	virtual void DrawHUD() override;
	void HandleLeftMouseButton(bool clicked);
	void SelectAllVisibleUnits();
	void IgnoreNextUpdate();
private:
	void DrawBox(const FVector2D& currentMousePosition);
	void UpdateSelection(const FVector2D& currentMousePosition);
	void UpdateSelectionAllVisible();
	bool MouseHasMovedSinceStart(const FVector2D& currentMousePosition) const;
	void SetSelectables(const TArray<AActor*> actors);
};

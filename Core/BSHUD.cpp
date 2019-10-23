// Fill out your copyright notice in the Description page of Project Settings.

#include "BSHUD.h"
#include <GameFramework/PlayerController.h>
#include "Components/BSSelectableComponent.h"
#include <Engine/Engine.h>

void ABSHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Enabled)
	{

		if (CurrentSelectables.Num() > 0)
		{	
			CurrentSelectables.Empty();
			if (OnSelectionUpdate.IsBound())
				OnSelectionUpdate.Broadcast(CurrentSelectables);
		}
		return;
	}

	if (SelectAllVisible)
	{
		UpdateSelectionAllVisible();
		return;
	}

	if (justDoubleClicked)
	{
		SendSelectionUpdate = false;
		DoSelectionUpdate = false;
		const float timePassed = GetWorld()->GetRealTimeSeconds() - timeOfDoubleClick;
		if (timePassed > DOUBLE_CLICK_COOLDOWN)
			justDoubleClicked = false;
	}

	if (!MouseDown && !SendSelectionUpdate || justDoubleClicked) return;

	FVector2D currentMousePosition = FVector2D();
	GetOwningPlayerController()->GetMousePosition(currentMousePosition.X, currentMousePosition.Y);

	DrawBox(currentMousePosition);

	//Make sure not to reselect unless the mouse has moved
	if (MouseHasMovedSinceStart(currentMousePosition)) DoSelectionUpdate = true;
	LatestMousePosition = currentMousePosition;
	
	if (!DoSelectionUpdate)
		return;

	UpdateSelection(currentMousePosition);
}
void ABSHUD::HandleLeftMouseButton(bool clicked)
{
	if (ignoreNext)
	{
		ignoreNext = false;
		return;
	}


	MouseDown = clicked;
	if (clicked)
	{
		firstInputGiven = true;
		GetOwningPlayerController()->GetMousePosition(StartMousePosition.X, StartMousePosition.Y);
		LatestMousePosition = StartMousePosition;
	}
	else
	{
		if (!firstInputGiven)
		{
			FVector2D currentMousePosition = FVector2D();
			GetOwningPlayerController()->GetMousePosition(currentMousePosition.X, currentMousePosition.Y);
			StartMousePosition = currentMousePosition;

		}

		DoSelectionUpdate = true;
		SendSelectionUpdate = true;
	}
}
void ABSHUD::SelectAllVisibleUnits()
{
	SelectAllVisible = true;
}

void ABSHUD::IgnoreNextUpdate()
{
	ignoreNext = true;
	SendSelectionUpdate = false;
	DoSelectionUpdate = false;
}

void ABSHUD::DrawBox(const FVector2D& currentMousePosition)
{
	const float width = currentMousePosition.X - StartMousePosition.X;
	const float height = currentMousePosition.Y - StartMousePosition.Y;
	DrawRect(BoxColor, StartMousePosition.X, StartMousePosition.Y, width, height);
}
void ABSHUD::UpdateSelection(const FVector2D& currentMousePosition)
{
	TArray<AActor*> outActors;
	GetActorsInSelectionRectangle<AActor>(StartMousePosition, currentMousePosition, outActors, false, false);
	int32 screenWidth;
	int32 screenHeight;
	PlayerOwner->GetViewportSize(screenWidth, screenHeight);
	const int32 size = outActors.Num();
	for (int32 i = size - 1; i >= 0; i--)
	{
		FVector2D screenLocation;
		bool projectedToScreen = PlayerOwner->ProjectWorldLocationToScreen(outActors[i]->GetActorLocation(), screenLocation);
		if (!projectedToScreen || screenLocation.X < 0 || screenLocation.Y < 0 || screenLocation.X > screenWidth || screenLocation.Y > screenHeight)
		{
			outActors.RemoveAt(i);
		}
	}
	SetSelectables(outActors);
}
void ABSHUD::UpdateSelectionAllVisible()
{
	SelectAllVisible = false;
	SendSelectionUpdate = true;
	
	if (GEngine == nullptr || GEngine->GameViewport == nullptr)
		return;
	justDoubleClicked = true;
	timeOfDoubleClick = GetWorld()->GetRealTimeSeconds();

	FVector2D screenSize;
	GEngine->GameViewport->GetViewportSize(screenSize);
	
	TArray<AActor*> outActors;
	GetActorsInSelectionRectangle<AActor>(FVector2D::ZeroVector, screenSize, outActors, false, false);
	SetSelectables(outActors);
}
bool ABSHUD::MouseHasMovedSinceStart(const FVector2D& currentMousePosition) const
{
	const float mouseMovement = (currentMousePosition - LatestMousePosition).SizeSquared();
	return mouseMovement < MinimumMovementToReselect * MinimumMovementToReselect;
}
void ABSHUD::SetSelectables(const TArray<AActor*> actors)
{
	for (UBSSelectableComponent* s : CurrentSelectables)
	{
		if (s->OnDeselect.IsBound())
			s->OnDeselect.Broadcast();
	}
	CurrentSelectables.Empty();

	const int32 size = actors.Num();
	for (int32 i = 0; i < size; i++)
	{
		UBSSelectableComponent* selectableComponent = Cast<UBSSelectableComponent>(actors[i]->GetComponentByClass(UBSSelectableComponent::StaticClass()));
		if (selectableComponent != nullptr && selectableComponent->CanBeSelected)
			CurrentSelectables.Add(selectableComponent);
	}
	for (UBSSelectableComponent* s : CurrentSelectables)
	{
		if (s->OnPreSelect.IsBound())
			s->OnPreSelect.Broadcast();
	}

	if (!MouseDown)
	{
		for (UBSSelectableComponent* s : CurrentSelectables)
		{
			if (s->OnSelect.IsBound())
				s->OnSelect.Broadcast();
		}
	}
		if (!SendSelectionUpdate)
			return;
		if (OnSelectionUpdate.IsBound())
			OnSelectionUpdate.Broadcast(CurrentSelectables);

	SendSelectionUpdate = false;
}
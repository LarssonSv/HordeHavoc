// Fill out your copyright notice in the Description page of Project Settings.

#include "BSPlayerController.h"
#include <Components/InputComponent.h>
#include "BSHUD.h"
#include "Components/BSSelectableComponent.h"
#include <EngineUtils.h>
#include <UObjectIterator.h>
#include <Kismet/GameplayStatics.h>
#include <NavigationSystem.h>

ABSPlayerController::ABSPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	Hud = Cast<ABSHUD>(GetHUD());
	Hud->OnSelectionUpdate.AddDynamic(this, &ABSPlayerController::UpdateSelection);
}

void ABSPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (RightClickDown && GetWorld()->GetRealTimeSeconds() - TimeOfLastMoveCommand > MOVE_COMMAND_COOLDOWN)
		SetMoveLocation();
}

void ABSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("LeftClick", IE_Pressed, this, &ABSPlayerController::HandleLeftClickPressed);
	InputComponent->BindAction("LeftClick", IE_Released, this, &ABSPlayerController::HandleLeftClickReleased);
	InputComponent->BindAction("LeftClick", IE_DoubleClick, this, &ABSPlayerController::HandleLeftDoubleClickPressed);

	InputComponent->BindAction("RightClick", IE_Pressed, this, &ABSPlayerController::HandleRightClickPressed);
	InputComponent->BindAction("RightClick", IE_Released, this, &ABSPlayerController::HandleRightClickReleased);

	InputComponent->BindAction("SelectionModifier", IE_Pressed, this, &ABSPlayerController::HandleSelectionModifierPressed);
	InputComponent->BindAction("SelectionModifier", IE_Released, this, &ABSPlayerController::HandleSelectionModifierReleased);
	InputComponent->BindAction("SelectAll", IE_Pressed, this, &ABSPlayerController::HandleSelectAllPressed);
	InputComponent->BindAction("SelectClosestIdle", IE_Pressed, this, &ABSPlayerController::HandleSelectClosestIdlePressed);

	InputComponent->BindAction("Restart", IE_DoubleClick, this, &ABSPlayerController::HandleRestartPressed);

}
void ABSPlayerController::HandleLeftClickPressed()
{
	Hud->HandleLeftMouseButton(true);
}
void ABSPlayerController::HandleLeftDoubleClickPressed()
{
	return;
	FVector worldLocation;
	FVector worldDirection;
	DeprojectMousePositionToWorld(worldLocation, worldDirection);
	FHitResult hit;
	GetWorld()->LineTraceSingleByChannel(hit, worldLocation, worldLocation + worldDirection * 10000, ECC_WorldDynamic);
	if (!hit.bBlockingHit || hit.GetActor() == nullptr)
		return;
	UBSSelectableComponent* selectable = Cast<UBSSelectableComponent>(hit.GetActor()->GetComponentByClass(UBSSelectableComponent::StaticClass()));
	if (selectable != nullptr)
		Hud->SelectAllVisibleUnits();
}
void ABSPlayerController::HandleLeftClickReleased()
{
	Hud->HandleLeftMouseButton(false);
}
void ABSPlayerController::HandleRightClickPressed()
{
	RightClickDown = true;
	ShowMovementIndicator = true;
	SetMoveLocation();
}
void ABSPlayerController::HandleRightClickReleased()
{
	RightClickDown = false;
}
void ABSPlayerController::HandleSelectionModifierPressed()
{
	SelectionModifierPressed = true;
}
void ABSPlayerController::HandleSelectionModifierReleased()
{
	SelectionModifierPressed = false;
}
void ABSPlayerController::HandleRestartPressed()
{
	UGameplayStatics::OpenLevel(GetWorld(), "?Restart");
}

void ABSPlayerController::HandleSelectClosestIdlePressed()
{
	FVector cameraLocation = GetPawn()->GetActorLocation();
	TArray<UBSSelectableComponent*> selectables;
	for (TActorIterator<AActor> allActors(GetWorld()); allActors; ++allActors)
	{
		UBSSelectableComponent* component = Cast<UBSSelectableComponent>(allActors->GetComponentByClass(UBSSelectableComponent::StaticClass()));
		if (component != nullptr && component->CanBeSelected && !Selectables.Contains(component))
		selectables.Add(component);
	}
	if (selectables.Num() == 0)
		return;
	
	selectables.Sort([cameraLocation](const UBSSelectableComponent& A, const UBSSelectableComponent& B)
	{
		const float aDistance = (A.GetOwner()->GetActorLocation() - cameraLocation).SizeSquared();
		const float bDistance = (B.GetOwner()->GetActorLocation() - cameraLocation).SizeSquared();
		return aDistance < bDistance;
	});
	UBSSelectableComponent* closestSelectable = selectables[0];
	selectables.Empty();
	selectables.Add(closestSelectable);
	UpdateSelection(selectables);
}

void ABSPlayerController::HandleSelectAllPressed()
{
	TArray<UBSSelectableComponent*> selectables;
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		UBSSelectableComponent* component = Cast<UBSSelectableComponent>(ActorItr->GetComponentByClass(UBSSelectableComponent::StaticClass()));
		if (component != nullptr && component->CanBeSelected)
			selectables.Add(component);
	}
	UpdateSelection(selectables);
}

void ABSPlayerController::SetMoveLocation()
{
	TimeOfLastMoveCommand = GetWorld()->GetRealTimeSeconds();
	const int32 size = Selectables.Num();
	if (size == 0)
		return;

	FVector worldLocation;
	FVector worldDirection;
	DeprojectMousePositionToWorld(worldLocation, worldDirection);
	FHitResult hit;
	GetWorld()->LineTraceSingleByChannel(hit, worldLocation, worldLocation + worldDirection * 10000, ECC_Visibility);
	if (!hit.bBlockingHit)
		return;



	//Move Graphics
	if (MoveCommandIndicator != nullptr && ShowMovementIndicator)
	{
		FVector location = hit.ImpactPoint;
		GetWorld()->SpawnActor(MoveCommandIndicator, &location);
		ShowMovementIndicator = false;
	}

	FVector Location;
	const FVector targetlocation = hit.Location;
	currentDistance = 200.f;
	currentAngle = 0.f;
	angleStep = 40.0f;
	int safetyCounter = 0;
	for (int32 i = 0; i < size;)
	{
		safetyCounter++;
		if (safetyCounter > 50)
		{
			i++;
			safetyCounter = 0;
			continue;
		}
		if (i == 0)
		{
			Location = targetlocation;
			if (Selectables[i]->OnMoveCommand.IsBound())
				Selectables[i]->OnMoveCommand.Broadcast(Location);
			i++;
			continue;
		}
		FVector ProjectedLocation;
		FVector Location = targetlocation + FVector((currentDistance * FMath::RandRange(0.9f, 1.2f)) * FMath::Sin(FMath::DegreesToRadians(currentAngle)), currentDistance * FMath::Cos(FMath::DegreesToRadians(currentAngle)), 0);
		currentAngle += angleStep;
		if (currentAngle > 360 - angleStep)
		{
			currentAngle -= 360 - angleStep;
			currentDistance += DISTANCE_STEP;
			angleStep *= AngleFactor;
		}
		
		if (!UNavigationSystemV1::K2_ProjectPointToNavigation(GetWorld(), Location, ProjectedLocation, nullptr, TSubclassOf<UNavigationQueryFilter>()))
			continue;

		if (Selectables[i]->OnMoveCommand.IsBound())
			Selectables[i]->OnMoveCommand.Broadcast(Location);
		i++;
		safetyCounter = 0;
	}
}

void ABSPlayerController::UpdateSelection(TArray<UBSSelectableComponent*> selectables)
{
	//Deselect current selected
	for (UBSSelectableComponent* s : Selectables)
	{
		if (s->OnDeselect.IsBound())
			s->OnDeselect.Broadcast();
	}
	//If normal select, just select the new ones
	if (!SelectionModifierPressed)
	{
		Selectables = selectables;
	}
	else
	{
		//If shift is down, add new selectables to the selection
		for (UBSSelectableComponent* s : selectables)
		{
			if (!Selectables.Contains(s))
				Selectables.Add(s);
		}
	}
	//Select all new selectables
	for (UBSSelectableComponent* s : Selectables)
	{
		if (s->OnSelect.IsBound())
			s->OnSelect.Broadcast();
	}
	if (OnSelectionUpdate.IsBound())
		OnSelectionUpdate.Broadcast();
	FVector center;
	if (!GetSelectedCenter(center))
		return;

}

void ABSPlayerController::Deselect()
{
	const int32 size = Selectables.Num();
	for (int32 i = size - 1; i >= 0; i--)
	{
		if (!Selectables[i]->CanBeSelected)
		{
			if (Selectables[i]->OnDeselect.IsBound())
				Selectables[i]->OnDeselect.Broadcast();
			Selectables.RemoveAt(i);
		}
	}
}

bool ABSPlayerController::GetSelectedCenter(FVector& outCenter) const
{
	const int32 size = Selectables.Num();
	if (size == 0)
		return false;
	outCenter = FVector::ZeroVector;
	for (UBSSelectableComponent* s : Selectables)
	{
		outCenter += s->GetOwner()->GetActorLocation();
	}
	outCenter /= size;
	return true;
}

void ABSPlayerController::SelectNearestOrc()
{
	HandleSelectClosestIdlePressed();
	Hud->IgnoreNextUpdate();
}

void ABSPlayerController::IgnoreNextHudInput()
{
	Hud->IgnoreNextUpdate();
}


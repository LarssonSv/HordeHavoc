#include "BSSelectableComponent.h"
#include <GameFramework/Actor.h>
#include <Components/StaticMeshComponent.h>
#include "Core/BSPlayerController.h"


UBSSelectableComponent::UBSSelectableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UBSSelectableComponent::BeginPlay()
{
	Super::BeginPlay();	
	TargetLocation = GetOwner()->GetActorLocation();
	canBeSelectedValue = CanBeSelected;
}
void UBSSelectableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (canBeSelectedValue != CanBeSelected)
	{
		canBeSelectedValue = CanBeSelected;
		if (!canBeSelectedValue)
		{
			ABSPlayerController* playerController = Cast<ABSPlayerController>(GetWorld()->GetFirstPlayerController());
			playerController->Deselect();
		}
	}

	const FVector delta = TargetLocation - GetOwner()->GetActorLocation();
	if(delta.Size() > StopDistance)
		GetOwner()->AddActorWorldOffset(delta.GetSafeNormal() * Speed * DeltaTime);
}

void UBSSelectableComponent::Deactivate()
{
	CanBeSelected = false;
	canBeSelectedValue = false;
}

void UBSSelectableComponent::ReActivate()
{
	CanBeSelected = true;
	canBeSelectedValue = true;
}

void UBSSelectableComponent::TempMove(FVector location)
{
	TargetLocation = location;
}

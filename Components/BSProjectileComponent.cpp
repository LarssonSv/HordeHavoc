#include "BSProjectileComponent.h"
#include <DrawDebugHelpers.h>
#include "Core/BSAIAgent.h"
#include "BSSelectableComponent.h"

UBSProjectileComponent::UBSProjectileComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}
void UBSProjectileComponent::BeginPlay()
{
	Super::BeginPlay();
}
void UBSProjectileComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Activated) return;

	Velocity -= FVector::UpVector * Gravity * DeltaTime;
	FHitResult hit;
	GetOwner()->AddActorWorldOffset(Velocity * DeltaTime, true, &hit);

	if (hit.bBlockingHit)
	{
		if (OnProjectileHit.IsBound())
			OnProjectileHit.Broadcast(hit, Velocity);


		if (ABSAIAgent* Orc = Cast<ABSAIAgent>(GetOwner()))
		{
			Orc->ActivateUnit();


			if (UBSSelectableComponent* selectComp = Orc->GetSelectableComp())
			{
				selectComp->ReActivate();
			}
		}

		Activated = false;
	}


}

void UBSProjectileComponent::SetTarget(const FVector& worldLocation)
{
	const FVector Spacinglocation = worldLocation + FVector(FMath::RandRange(0.0f, Accuracy), FMath::RandRange(0.0f, Accuracy),0.0f);

	TargetLocation = Spacinglocation;
	StartLocation = GetOwner()->GetActorLocation();
	FVector toLocation = TargetLocation - StartLocation;
	const float deltaZ = TargetLocation.Z - StartLocation.Z;
	toLocation.Z = 0.0f;
	const float planarDistance = toLocation.Size2D();
	TrajectoryRotation = toLocation.Rotation();


	AngleInRads = FMath::Atan((deltaZ + 0.5f * Gravity * TimeToImpact * TimeToImpact) / planarDistance);
	InitialVelocity = planarDistance / (TimeToImpact * FMath::Cos(AngleInRads));
	TrajectoryRotation.Pitch = FMath::RadiansToDegrees(AngleInRads);
	Velocity = TrajectoryRotation.Vector() * InitialVelocity;

	Activated = true;

	if (OnProjectileFire.IsBound())
		OnProjectileFire.Broadcast(TrajectoryRotation);
	
	
	//Draw target location 
	//DrawDebugBox(GetWorld(), TargetLocation, FVector(100, 100, 100), FQuat::Identity, FColor::Red, true, 10.0f, 0, 3);
}


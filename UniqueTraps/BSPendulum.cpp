#include "BSPendulum.h"
#include <TransformCalculus2D.h>
#include <Components/StaticMeshComponent.h>
#include <DrawDebugHelpers.h>
#include <Kismet/KismetMathLibrary.h>
#include <Components/BoxComponent.h>

ABSPendulum::ABSPendulum()
{
	PrimaryActorTick.bCanEverTick = true;
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	RootComponent = CollisionBox;
	
}
void ABSPendulum::BeginPlay()
{
	Super::BeginPlay();
	Velocity = FVector::ZeroVector;
	Origin = GetActorLocation() + FVector::UpVector * ArmLength;
	FVector right = GetActorRightVector();
	FVector startLocation = Origin + (right.RotateAngleAxis(-90 + StartAngle, GetActorForwardVector()) * ArmLength);
	SetActorLocation(startLocation);
	Right = GetActorRightVector();
}
void ABSPendulum::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Velocity -= FVector::UpVector * Gravity * DeltaTime;
	FVector normal = (Origin - GetActorLocation()).GetSafeNormal();	
	const float dot = FVector::DotProduct(normal, Velocity);
	const FVector normalForce = -1 * normal * dot;
	Velocity += normalForce;

	AddActorWorldOffset(Velocity * DeltaTime);

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (Origin - GetActorLocation()).GetSafeNormal() * ArmLength, FColor::Red, false, 0.0f, 0, 3.0f);
	DrawDebugSphere(GetWorld(), Origin, 25.0f, 12, FColor::Red, false, 0.0f, 0, 3.0f);

	FVector toOrigin = Origin - GetActorLocation();
	if (toOrigin.Size() > ArmLength + SMALL_VALUE)
	{
		SetActorLocation(Origin - toOrigin.GetSafeNormal() * ArmLength);
	}

	if (!ApplyRotation)
		return;

	//ROTATION
	toOrigin = (GetActorLocation() - Origin).GetSafeNormal();
	const float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(toOrigin, -FVector::UpVector)));
	const float direction = FMath::Sign(FVector::DotProduct(Right, toOrigin)) * -1.0f;

	FRotator rotator = GetActorRotation();
	rotator.Roll = angle * direction;
	SetActorRotation(rotator);
}

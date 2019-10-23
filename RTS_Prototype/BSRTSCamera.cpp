#include "BSRTSCamera.h"
#include <Kismet/GameplayStatics.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Components/SceneComponent.h>
#include "Engine/TriggerBox.h"
#include <EngineUtils.h>
#include "Core/BSPlayerController.h"
#include "Components/BSExplodableComponent.h"
#include "Components/BSExplosiveComponent.h"
#include "DrawDebugHelpers.h"

ABSRTSCamera::ABSRTSCamera()
{
	PrimaryActorTick.bCanEverTick = true;
	CameraAttachmentPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RootPoint"));
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	RootComponent = CameraAttachmentPoint;
	CameraArm->SetupAttachment(CameraAttachmentPoint);
	Camera->SetupAttachment(CameraArm);
}
void ABSRTSCamera::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<ABSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	PlayerController->OnSelectionUpdate.AddDynamic(this, &ABSRTSCamera::SetFocusInput);
	for (TActorIterator<ATriggerBox> it(GetWorld()); it; ++it)
	{
		if (it->GetName().Equals(BoundingBoxName))
			BoundingBox = *it;
	}
	TargetHeight = GetActorLocation().Z;
	StartRotation = GetActorRotation();
}
void ABSRTSCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Activated) return;
	UpdateRotation(DeltaTime);
	UpdateZoom(DeltaTime);
	UpdateMovement(DeltaTime);
	FollowGroundLevel(DeltaTime);

}
void ABSRTSCamera::SetupPlayerInputComponent(UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	InputComponent->BindAxis("Rotate", this, &ABSRTSCamera::SetRotationInput);
	InputComponent->BindAxis("Horizontal", this, &ABSRTSCamera::SetHorizontalInput);
	InputComponent->BindAxis("Vertical", this, &ABSRTSCamera::SetVerticalInput);
	InputComponent->BindAxis("Zoom", this, &ABSRTSCamera::SetZoomInput);
	InputComponent->BindAction("Focus", IE_Pressed, this, &ABSRTSCamera::SetFocusInput);
}
void ABSRTSCamera::UpdateRotation(const float DeltaTime)
{
	const FRotator rotator = FRotator(0, RotationInput * RotationSpeed * DeltaTime, 0);

	if (GetActorRotation().Yaw + rotator.Yaw < StartRotation.Yaw + ClampAngle && GetActorRotation().Yaw + rotator.Yaw > StartRotation.Yaw + -ClampAngle)
	{
		AddActorLocalRotation(rotator);
	}
}

void ABSRTSCamera::UpdateZoom(const float DeltaTime)
{
	const float zoom = ZoomInput * ZoomSpeed * DeltaTime;
	float currentLength = CameraArm->TargetArmLength;
	currentLength += zoom;
	currentLength = FMath::Clamp(currentLength, MinDistance, MaxDistance);
	CameraArm->TargetArmLength = currentLength;
}
void ABSRTSCamera::UpdateMovement(const float DeltaTime)
{
	//Check input
	float mouseX = 0.0f, mouseY = 0.0f;
	int32 viewportSizeX = 0, viewportSizeY = 0;
	PlayerController->GetMousePosition(mouseX, mouseY);
	PlayerController->GetViewportSize(viewportSizeX, viewportSizeY);

	const float deltaX = viewportSizeX - mouseX;
	const float deltaY = viewportSizeY - mouseY;

	int horizontal = 0, vertical = 0;

	if (deltaX < WidthBuffer)
		horizontal = 1;
	else if (deltaX > viewportSizeX - WidthBuffer)
		horizontal = -1;

	if (deltaY < HeightBuffer)
		vertical = -1;
	else if (deltaY > viewportSizeY - HeightBuffer)
		vertical = 1;
	if (FMath::Sign(HorizontalInput) != 0) horizontal = HorizontalInput;
	if (FMath::Sign(VerticalInput) != 0) vertical = VerticalInput;
	horizontal = FMath::Clamp(horizontal, -1, 1);
	vertical = FMath::Clamp(vertical, -1, 1);

	//Check if giving input
	const FVector input = FVector(vertical, horizontal, 0.0f);
	const bool givingInput = (input.SizeSquared() > MINIMUM_ACCEPTED_INPUT * MINIMUM_ACCEPTED_INPUT);
	if (givingInput)
	{
		FocusOnUnits = false;
		MovementDirection = input.GetSafeNormal();
	}
	else if (FocusOnUnits)
	{
		FollowUnits(DeltaTime);
	}

	//Apply movement
	CurrentSpeed += (givingInput ? Acceleration : -Deceleration) * DeltaTime;
	CurrentSpeed = FMath::Clamp(CurrentSpeed, 0.0f, Speed);
	AddActorLocalOffset(MovementDirection * CurrentSpeed * DeltaTime);
	//Bounding box
	if (BoundingBox == nullptr)
		return;
	FVector center, bounds;
	BoundingBox->GetActorBounds(false, center, bounds);
	FVector location = GetActorLocation();
	//TODO: rotate bounds x and y
	location.X = FMath::Clamp(location.X, center.X - bounds.X, center.X + bounds.X);
	location.Y = FMath::Clamp(location.Y, center.Y - bounds.Y, center.Y + bounds.Y);
	location.Z = GetActorLocation().Z;
	SetActorLocation(location);
}
void ABSRTSCamera::FollowUnits(const float DeltaTime)
{
	FVector centerPoint;
	bool unitsSelected = PlayerController->GetSelectedCenter(centerPoint);
	centerPoint.Z = GetActorLocation().Z;
	if (unitsSelected)
	{
		const FVector toTarget = centerPoint - GetActorLocation();
		FVector newLocation = GetActorLocation() + toTarget * FMath::Clamp(FollowCameraSmoothing * DeltaTime, 0.0f, 1.0f);
		SetActorLocation(newLocation);
	}
	else
		FocusOnUnits = false;
	
}

void ABSRTSCamera::SetRotationInput(float rotation)
{
	RotationInput = rotation;
}
void ABSRTSCamera::SetHorizontalInput(float horizontal)
{
	HorizontalInput = horizontal;
}
void ABSRTSCamera::SetVerticalInput(float vertical)
{
	VerticalInput = vertical;
}
void ABSRTSCamera::SetZoomInput(float zoom)
{
	ZoomInput = zoom;
}

void ABSRTSCamera::FollowGroundLevel(float DeltaTime)
{
	const FVector startLocation = GetActorLocation() + FVector(0, 0, 2000.f);
	const FVector endLocation = startLocation - FVector(0, 0, 4000.f);
	FHitResult hit;
	GetWorld()->LineTraceSingleByChannel(hit, startLocation, endLocation, ECC_Visibility);
	if (hit.bBlockingHit)
	{
		TargetHeight = hit.Location.Z + 50.f;
	}
	FVector currentLocation = GetActorLocation();
	currentLocation.Z = FMath::Lerp(GetActorLocation().Z, TargetHeight, LerpSpeed * DeltaTime);
	SetActorLocation(currentLocation);
}

void ABSRTSCamera::ExplodeShake()
{
	//GetWorld()->GetFirstPlayerController()->PlayerCameraManager->PlayCameraShake(ExplosionShake, 1.0f);
}

void ABSRTSCamera::SetFocusInput()
{
	FocusOnUnits = true;
}


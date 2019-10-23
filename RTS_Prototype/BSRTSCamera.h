// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BSElementComponentSystem.h"
#include "BSRTSCamera.generated.h"

UCLASS()
class TEAM2_PROJECT2_API ABSRTSCamera : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	class USceneComponent* CameraAttachmentPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	class USpringArmComponent* CameraArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Components)
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float HeightBuffer = 15.0f;
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float WidthBuffer = 15.0f;
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float Speed = 1000;
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float Acceleration = 100;
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float Deceleration = 100;
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float FollowCameraSmoothing = 1000;
	UPROPERTY(EditDefaultsOnly, Category = Movement)
	float RotationSpeed = 1000;
	UPROPERTY(EditDefaultsOnly, Category = Zoom)
	float ZoomSpeed = 1000;
	UPROPERTY(EditDefaultsOnly, Category = Zoom)
	float MinDistance = 300.0f;
	UPROPERTY(EditDefaultsOnly, Category = Zoom)
	float MaxDistance = 1000.0f;
	UPROPERTY(EditDefaultsOnly, Category = Zoom)
	float ClampAngle = 35.0f;
	UPROPERTY(EditDefaultsOnly)
	float LerpSpeed = 40.f;
	UPROPERTY(EditDefaultsOnly, Category = Bounding)
	FString BoundingBoxName = "BoundingBox";

	UPROPERTY(EditDefaultsOnly, Category = CameraShake)
	TSubclassOf<UCameraShake> ExplosionShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Activated = true;

private:
	class ABSPlayerController* PlayerController;
	float RotationInput;
	float HorizontalInput;
	float VerticalInput;
	float ZoomInput;
	float CurrentSpeed;
	FVector MovementDirection;
	float TargetHeight;
	class ATriggerBox* BoundingBox = nullptr;
	bool FocusOnUnits;
	const float MINIMUM_ACCEPTED_INPUT = 0.1f;
	FRotator StartRotation;
public:
	ABSRTSCamera();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	void ExplodeShake();
private:
	void UpdateRotation(const float DeltaTime);
	void UpdateZoom(const float DeltaTime);
	void UpdateMovement(const float DeltaTime);
	void FollowUnits(const float DeltaTime);
	void SetRotationInput(float rotation);
	void SetHorizontalInput(float horizontal);
	void SetVerticalInput(float vertical);
	void SetZoomInput(float zoom);
	void FollowGroundLevel(float DeltaTime);
	
	UFUNCTION()
	void SetFocusInput();

};

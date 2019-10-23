// Fill out your copyright notice in the Description page of Project Settings.
// Author: Simon o Pär o Suat(he fixed it!)
#include "BSCameraShaker.h"
#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/Actor.h>
#include <Camera/CameraComponent.h>

UBSCameraShaker::UBSCameraShaker()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UBSCameraShaker::BeginPlay()
{
	Super::BeginPlay();

	World = GetWorld();
	CameraRef = Cast<UCameraComponent>(GetOwner()->GetComponentByClass(UCameraComponent::StaticClass()));
}

void UBSCameraShaker::AddCameraShake(float x)
{
	CurrentIntensity += x;
}

void UBSCameraShaker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Decrease CurrentIntensity every deltaframe
	CurrentIntensity -= DeltaTime / MaxDuration;
	CurrentIntensity = FMath::Clamp(CurrentIntensity, 0.0f, 1.0f);
	//Also clamp it 0-1 so ppl cant go overboard

	//Get xPerlin and multiple with how fast we want the shake to be
	float xPerlin = (UKismetMathLibrary::PerlinNoise1D(World->GetRealTimeSeconds() * PerlinSpeed.X) + 1) * 0.5f;
	float xShake = FMath::Lerp(-MaxShake.X, MaxShake.X, xPerlin);

	float yPerlin = (UKismetMathLibrary::PerlinNoise1D(World->GetRealTimeSeconds() * PerlinSpeed.Y) + 1) * 0.5f;
	float yShake = FMath::Lerp(-MaxShake.Y, MaxShake.Y, yPerlin);

	float zPerlin = (UKismetMathLibrary::PerlinNoise1D(World->GetRealTimeSeconds() * PerlinSpeed.Z) + 1) * 0.5f;
	float zShake = FMath::Lerp(-MaxShake.Z, MaxShake.Z, zPerlin);

	//For Rotation Roll

	float rollIntense = (UKismetMathLibrary::PerlinNoise1D((World->GetRealTimeSeconds() * RollSpeed)) + 1) * 0.5f;
	float rollShake = FMath::Lerp(-MaxRoll, MaxRoll, rollIntense);


	//Set our cameracomp to it
	if (CameraRef)
	{
		CameraRef->SetRelativeLocation(FVector(xShake * CurrentIntensity, yShake * CurrentIntensity, zShake * CurrentIntensity));
		CameraRef->SetRelativeRotation(FRotator(0.0f, 0.0f, rollShake * CurrentIntensity));
	}
		

}


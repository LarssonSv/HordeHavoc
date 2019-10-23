// Fill out your copyright notice in the Description page of Project Settings.
// Author: Simon
#include "BSCatapultPreasure.h"
#include "Components/BSSelectableComponent.h"
#include <TimerManager.h>

ABSCatapultPreasure::ABSCatapultPreasure()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABSCatapultPreasure::BeginPlay()
{
	Super::BeginPlay();

	if (Collider != nullptr)
	{
		Collider->OnComponentBeginOverlap.AddDynamic(this, &ABSCatapultPreasure::BeginOverlap);
		Collider->OnComponentEndOverlap.AddDynamic(this, &ABSCatapultPreasure::EndOverlap);
		Collider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Collider->SetCollisionProfileName(FName("DeathComponent"));

		FTimerManager& TimerManager = GetWorldTimerManager();
		TimerManager.ClearTimer(untilNextLaunch);
		TimerManager.ClearTimer(delayedDisable);
	}
}

void ABSCatapultPreasure::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFrameSweep, const FHitResult& SweepResults)
{
	if (Active && Target != nullptr)
	{
		if (ABSAIAgent* Orc = Cast<ABSAIAgent>(OtherActor))
		{
			if (Orc->IsDead() != true)
			{
				FTimerManager& TimerManager = GetWorldTimerManager();

				FVector offset = Collider->GetCenterOfMass() += FVector(FMath::RandRange(-orcSpacing, orcSpacing), 0.0f, FMath::RandRange(-orcSpacing, orcSpacing));

				if (TimerManager.GetTimerRemaining(untilNextLaunch) <= 0.0f)
				{
					TimerManager.ClearTimer(untilNextLaunch);
					TimerManager.SetTimer(untilNextLaunch, this, &ABSCatapultPreasure::Launch, Time, false);

					if (OnDelayStart.IsBound())
						OnDelayStart.Broadcast();
				}

				if(!agentList.Contains(Orc))
				{
					//Incase the physic collision is missed by lowframerate we have list check aswell
					agentList.Add(Orc);
				}
				
			}
		}
	}
}

void ABSCatapultPreasure::Launch()
{

	for (int i = 0; i < agentList.Num(); i++)
	{
		if (UBSProjectileComponent* ProjectileComponent = agentList[i]->GetProjectileComp())
		{
			if (ProjectileComponent->Activated != true)
			{
				ProjectileComponent->SetTarget(Target->GetActorLocation());
			}
		}
		
	}
	agentList.Empty();

	if (OnProjectileFire.IsBound())
		OnProjectileFire.Broadcast();
}

void ABSCatapultPreasure::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (ABSAIAgent* Orc = Cast<ABSAIAgent>(OtherActor))
	{
		agentList.Remove(Orc);
		Orc->StopMovement();

	}
}


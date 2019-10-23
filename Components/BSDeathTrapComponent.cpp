// Fill out your copyright notice in the Description page of Project Settings.
//Author: Simon
#include "BSDeathTrapComponent.h"
#include "Core/BSAIAgent.h"

UBSDeathTrapComponent::UBSDeathTrapComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	

}

int32 UBSDeathTrapComponent::GetDeathCount() const
{
	return deathCount;
}

void UBSDeathTrapComponent::BeginPlay()
{
	Super::BeginPlay();


	for (int i = 0; i < DeathColliders.Num(); i++)
	{
		DeathColliders[i]->OnComponentBeginOverlap.AddDynamic(this, &UBSDeathTrapComponent::BeginOverlap);
		DeathColliders[i]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DeathColliders[i]->SetCollisionProfileName(FName("DeathComponent"));
	}



}

void UBSDeathTrapComponent::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFrameSweep, const FHitResult& SweepResults)
{
	if (trapActive)
	{
		if (ABSAIAgent* Orc = Cast<ABSAIAgent>(OtherActor))
		{
			if (Orc->IsDead() != true)
			{
				Orc->Die(DeathType);
				deathCount++;
				OnKill.Broadcast(OtherActor, SweepResults.ImpactPoint);
			}
		}
	}
}


// Fill out your copyright notice in the Description page of Project Settings.
// Author: Simon
#include "BSCannon.h"
#include "Components/BSSelectableComponent.h"
#include "Components/BSProjectileComponent.h"
ABSCannon::ABSCannon()
{
	PrimaryActorTick.bCanEverTick = true;

}

bool ABSCannon::LoadProjectile(ABSAIAgent* OrcToLoad, FVector Location)
{
	if (AmmoSlot != nullptr || OrcToLoad == nullptr) return false;

	if (OrcToLoad->isDisabled == false)
	{
		OrcToLoad->DisableUnit();
	}

	if (UBSSelectableComponent* SelectComp = OrcToLoad->GetSelectableComp())
	{
		SelectComp->Deactivate();
	}

	OrcToLoad->SetActorLocation(Location);

	AmmoSlot = OrcToLoad;

	return true;
}

bool ABSCannon::FireProjectile()
{
	if (AmmoSlot == nullptr) return false;

	AmmoSlot->Die();

	if (UBSProjectileComponent* ProjectileComponent = AmmoSlot->GetProjectileComp())
	{
		if (ProjectileComponent->Activated != true)
		{
			if (Target)
			{
				ProjectileComponent->SetTarget(Target->GetActorLocation());
			}
			
		}
	}

	return true;
}

bool ABSCannon::RotateCannon()
{
	return true;
}






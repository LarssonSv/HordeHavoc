#include "BSElementComponentSystem.h"
#include "Kismet/GameplayStatics.h"
#include "BSBurnableComponent.h"
#include "BSExplosiveComponent.h"
#include "DrawDebugHelpers.h"
#include "BSElementComponentBase.h"
#include "BSIceComponent.h"
#include "BSExplodableComponent.h"
#include "TimerManager.h"

ABSElementComponentSystem::ABSElementComponentSystem()
{
	PrimaryActorTick.bCanEverTick = true;

}
void ABSElementComponentSystem::BeginPlay()
{
	Super::BeginPlay();
	uidCounter = 0;
	UpdateTree();
}
void ABSElementComponentSystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	UpdateTree();
	UpdateSpreading();
	UpdateBurnables(DeltaTime);
	UpateExplosives(DeltaTime);
	UpdateFreezeables(DeltaTime);
	for (int32& id : ComponentsToDestroy)
		RemoveElementComponent(id); 
	if (DebugDrawEntireOrcTree)
		DrawOrcTree();
	ComponentsToDestroy.Empty();
}
void ABSElementComponentSystem::AddElementComponent(UBSElementComponentBase* component)
{
	const int32 id = uidCounter++;
	AllComponents.Add(id, component);
	UBSBurnableComponent* burnable = Cast<UBSBurnableComponent>(component);
	
	if (burnable != nullptr)
	{
		burnable->ComponentID = id;
		Burnables.Add(burnable->ComponentID, burnable);
		BurnableComponents.Add(burnable->ComponentID);
		return;
	}
	UBSExplosiveComponent* explosive = Cast<UBSExplosiveComponent>(component);
	if (explosive != nullptr)
	{
		explosive->ComponentID = id;
		Explosives.Add(explosive->ComponentID, explosive);
		ExplosiveComponents.Add(explosive->ComponentID);
		return;
	}
	UBSIceComponent* freezable = Cast<UBSIceComponent>(component);
	if (freezable != nullptr)
	{
		freezable->ComponentID = id;
		Freezables.Add(freezable->ComponentID, freezable);
		IceComponents.Add(freezable->ComponentID);
		return;
	}
	UBSExplodableComponent* explodable = Cast<UBSExplodableComponent>(component);
	if (explodable != nullptr)
	{
		explodable->ComponentID = id;
		Explodables.Add(explodable->ComponentID, explodable);
		ExplodableComponents.Add(explodable->ComponentID);
		return;
	}
}

void ABSElementComponentSystem::AddToComponentToDestroy(int32 id)
{
	ComponentsToDestroy.Add(id);
}

void ABSElementComponentSystem::RemoveElementComponent(int32 id)
{
	if (BurnableComponents.Contains(id))
	{
		BurnableComponents.Remove(id);
		if (Burnables.Contains(id))
			Burnables.Remove(id);
		if (IgnitionTimers.Contains(id))
			IgnitionTimers.Remove(id);
		if (DeathTimers.Contains(id))
			DeathTimers.Remove(id);
		if (InfiniteFires.Contains(id))
			InfiniteFires.Remove(id);
	}
	else if (ExplosiveComponents.Contains(id))
	{
		ExplosiveComponents.Remove(id);
		if (Explosives.Contains(id)) 
			Explosives.Remove(id);
		if (PrimeTimers.Contains(id)) 
			PrimeTimers.Remove(id);
		if (LocationsOfLastTrail.Contains(id))
			LocationsOfLastTrail.Remove(id);
	}
	else if (IceComponents.Contains(id))
	{
		IceComponents.Remove(id);
		if (Freezables.Contains(id))
			Freezables.Remove(id);
		if (FreezeTimers.Contains(id))
			FreezeTimers.Remove(id);
	}
	else if (ExplodableComponents.Contains(id))
	{
		ExplodableComponents.Remove(id);
		if (Explodables.Contains(id))
			Explodables.Remove(id);
		
	}
	if (AllComponents.Contains(id))
		AllComponents.Remove(id);
}

void ABSElementComponentSystem::UpdateTree()
{
	if (OrcTree != nullptr) 
		OrcTree->Destroy();

	const FVector min = FVector::OneVector * -Extent;
	const FVector max = FVector::OneVector * Extent;
	OrcTree = new FOrcTree(GetActorLocation(), FBox(min, max).GetExtent().GetMax());

	for (TPair<int32, UBSElementComponentBase*> pair : AllComponents)
	{
		if (!IsValid(pair.Value) || !IsValid(pair.Value->GetOwner()))
		{
			ComponentsToDestroy.Add(pair.Key);
			continue;
		}

		const int32 id = pair.Key;;
		FOctTreeElement element;
		element.Id = id;
		element.MyActor = AllComponents[id]->GetOwner();
		FVector origin;
		FVector extent;
		AllComponents[id]->GetOwner()->GetActorBounds(true, origin, extent);
		const float radius = FMath::Max(MINIMUM_RADIUS, FMath::Min(extent.X, extent.Y));
		origin = AllComponents[id]->GetOwner()->GetActorLocation();
		const FBoxSphereBounds boxSphereBounds = GetBoxSphereFromRadius(origin, radius);
		element.BoxSphereBounds = boxSphereBounds;
		OrcTree->AddElement(element);
	}
	for (int32& id : ComponentsToDestroy)
	{
		RemoveElementComponent(id);
	}
	ComponentsToDestroy.Empty();
}
void ABSElementComponentSystem::UpdateSpreading()
{
	CurrentlyIgniting.Empty();
	CurrentlyFreezing.Empty();
	CheckedComponents.Empty();
	TArray<int32> foundElements;
	for (int32& id : BurnableComponents)
	{
		//If burning, check for nearby burnables and ignite them
		if (Burnables[id]->IsBurning)
		{
			UBSBurnableComponent* burnable = Burnables[id];
			GetElementsAtPosition(burnable->GetOwner()->GetActorLocation(), burnable->FireSpreadRadius, foundElements);
			for (int32 id2 : foundElements)
			{
				if (CheckedComponents.Contains(id2))
					continue;
				
				if (BurnableComponents.Contains(id2) && !Burnables[id2]->IsBurning)
				{
					CurrentlyIgniting.Add(id2);
					CheckedComponents.Add(id2);
				}
				if (ExplosiveComponents.Contains(id2) && Explosives[id2]->IsActivated)
				{
					if(Explosives[id2]->OnFuseIgnition.IsBound())
						Explosives[id2]->OnFuseIgnition.Broadcast();
					Explosives[id2]->IsPrimed = true;
				}
			}
		}
	}
	for (int32& id : IceComponents)
	{
		if (Freezables[id]->CanFreezeOthers)
		{
			GetElementsAtPosition(Freezables[id]->GetOwner()->GetActorLocation(), Freezables[id]->IceSpreadRadius, foundElements);
			for (int32& id2 : foundElements)
			{
				if (IceComponents.Contains(id2) && !Freezables[id2]->IsFrozen)
					CurrentlyFreezing.Add(id2);
			}
		}
	}
	for (int32& id : ExplosiveComponents)
	{
		if (Explosives[id]->CanSpreadOil)
		{
			if (OrcsWalkedInto.Contains(id) && OrcsWalkedInto[id] >= Explosives[id]->NumberOfSpreads)
			{
				Explosives[id]->CanSpreadOil = false;
				break;
			}
			
			GetElementsAtPosition(Explosives[id]->GetOwner()->GetActorLocation(), Explosives[id]->OilSpreadRadius, foundElements);
			for (int32 id2 : foundElements)
			{
				if (BurnableComponents.Contains(id2) && Burnables[id2]->IsBurning)
					continue;
				if (ExplosiveComponents.Contains(id2) && !Explosives[id2]->IsActivated)
				{	
					Explosives[id2]->IsActivated = true;
					if (OrcsWalkedInto.Contains(id))
						OrcsWalkedInto[id]++;
					else
						OrcsWalkedInto.Add(id, 1);
					if (Explosives[id]->OnOilSpread.IsBound())
						Explosives[id]->OnOilSpread.Broadcast(OrcsWalkedInto[id]);
					
					if (Explosives[id2]->OnActivated.IsBound())
						Explosives[id2]->OnActivated.Broadcast();
				}
			}
		}
	}
}
void ABSElementComponentSystem::UpdateBurnables(float DeltaTime)
{
	for (int32& id : BurnableComponents)
	{
		//just started ignition
		if (CurrentlyIgniting.Contains(id) && !IgnitionTimers.Contains(id))
		{
			IgnitionTimers.Add(id, 0.0f);
			if (Burnables[id]->OnStartedIgnition.IsBound())
				Burnables[id]->OnStartedIgnition.Broadcast();
		}
		//just stopped ignition
		if (!CurrentlyIgniting.Contains(id) && IgnitionTimers.Contains(id))
		{
			IgnitionTimers.Remove(id);
			if (Burnables[id]->OnStoppedIgnition.IsBound())
				Burnables[id]->OnStoppedIgnition.Broadcast();
		}
		//Tick ignition
		if (IgnitionTimers.Contains(id))
		{
			IgnitionTimers[id] += DeltaTime;
			if (IgnitionTimers[id] > Burnables[id]->TimeUntilBurning)
			{
				Burnables[id]->IsBurning = true;
				DeathTimers.Add(id);
				IgnitionTimers.Remove(id);
				if (Burnables[id]->OnStartedBurning.IsBound())
					Burnables[id]->OnStartedBurning.Broadcast();
			}
		}
		//If something started burning (i.e. from blueprint)
		if (Burnables[id]->IsBurning && !DeathTimers.Contains(id) && !InfiniteFires.Contains(id))
		{
			if (Burnables[id]->OnStartedBurning.IsBound())
				Burnables[id]->OnStartedBurning.Broadcast();
			if(IgnitionTimers.Contains(id)) IgnitionTimers.Remove(id);
			if (Burnables[id]->TimeUntilDeath == 0.0f)
				InfiniteFires.Add(id);
			else
				DeathTimers.Add(id);
		}
		//If something stopped burning (i.e from water)
		if (!Burnables[id]->IsBurning && DeathTimers.Contains(id))
		{
			DeathTimers.Remove(id);
			if (Burnables[id]->OnStoppedBurning.IsBound())
				Burnables[id]->OnStoppedBurning.Broadcast();
		}

		//Tick death
		if (DeathTimers.Contains(id))
		{
			DeathTimers[id] += DeltaTime;
			if (DeathTimers[id] > Burnables[id]->TimeUntilDeath)
			{
				ComponentsToDestroy.Add(id);
				if (Burnables[id]->OnBurnedDown.IsBound())
					Burnables[id]->OnBurnedDown.Broadcast();
			}
		}		
	}
}
void ABSElementComponentSystem::UpateExplosives(float DeltaTime)
{
	TArray<int32> Exploding;

	for (int32& id : ExplosiveComponents)
	{
		if (!Explosives[id]->IsActivated)
			continue;

		//Check if explosive should be leaving trail
		if (Explosives[id]->LeavesBurnableTrail && !LocationsOfLastTrail.Contains(id))
		{
			LocationsOfLastTrail.Add(id, Explosives[id]->GetOwner()->GetActorLocation());
		}
		//Check if a trail bit should be dropped
		if (LocationsOfLastTrail.Contains(id))
		{
			const FVector location = Explosives[id]->GetOwner()->GetActorLocation();
			const float distance = Explosives[id]->FirstTime ? Explosives[id]->FirstDistance : Explosives[id]->DistanceBetweenTrailParts;
			if ((location - LocationsOfLastTrail[id]).SizeSquared() > (distance * distance))
			{
				LocationsOfLastTrail[id] = location;
				Explosives[id]->FirstTime = false;
				if (Explosives[id]->OnTrailUpdate.IsBound())
					Explosives[id]->OnTrailUpdate.Broadcast(location);
			}
		}
		//Check if explosive should be lit
		if (Explosives[id]->IsPrimed)
		{
			if (!PrimeTimers.Contains(id))
			{
				PrimeTimers.Add(id, 0.0f);
				if (Explosives[id]->OnFuseIgnition.IsBound())
					Explosives[id]->OnFuseIgnition.Broadcast();
			}
		}
		//Check if explosive should explode
		if (PrimeTimers.Contains(id))
		{
			PrimeTimers[id] += DeltaTime;
			if (PrimeTimers[id] >= Explosives[id]->FuseTime)
				Exploding.Add(id);
		}
	}
	//Explode components
	TArray<int32> nearbyExplosives;
	TArray<int32> foundExplodables;
	TSet<int32> blast;
	for (int32& id : Exploding)
	{
		if(ComponentsToDestroy.Contains(id))
			continue;
		UBSExplosiveComponent* explosive = Explosives[id];
		bool foundExplosives = false;
		FVector epicenter = explosive->GetOwner()->GetActorLocation();
		float radius = explosive->BlastRadius;
		float power = explosive->ExplosivePower;
		int numberOfExplosives = 1;
		blast.Add(id);
		ComponentsToDestroy.Add(id);
		do
		{
			foundExplosives = false;
			GetElementsAtPosition(epicenter / numberOfExplosives, (radius/numberOfExplosives * Curve.GetRichCurve()->Eval(numberOfExplosives)), nearbyExplosives);
			for (int32& id2 : nearbyExplosives)
			{
				if (ExplosiveComponents.Contains(id2) && !blast.Contains(id2) && Explosives[id2]->IsActivated)
				{
					blast.Add(id2);
					epicenter += Explosives[id2]->GetOwner()->GetActorLocation();
					radius += Explosives[id2]->BlastRadius;
					power += Explosives[id2]->ExplosivePower;
					numberOfExplosives++;
					ComponentsToDestroy.Add(id2);
					foundExplosives = true;
				}
			}
		} while (foundExplosives);
		blast.Empty();

		if (Explosives[id]->OnExplosion.IsBound())
		{
			epicenter /= numberOfExplosives;
			radius = (radius / numberOfExplosives * Curve.GetRichCurve()->Eval(numberOfExplosives));
			
			GetElementsAtPosition(epicenter, radius, foundExplodables);
			for (int32& id2 : foundExplodables)
			{
				if (!Explodables.Contains(id2))
					continue;
				const FVector direction = (Explodables[id2]->GetOwner()->GetActorLocation() - epicenter).GetSafeNormal();
				const float minimumValue = power * Explosives[id]->MinimumExplosiveFactor;
				const float lerpValue = (Explodables[id2]->GetOwner()->GetActorLocation() - epicenter).Size() / radius;
				float actualPower = FMath::Lerp(power, minimumValue, FMath::Clamp(lerpValue, 0.f, 1.f));
				if(Explodables[id2]->OnExploded.IsBound())
					Explodables[id2]->OnExploded.Broadcast(actualPower, direction);
			}
			Explosives[id]->OnExplosion.Broadcast(epicenter, radius, power);
		}
	}
}
void ABSElementComponentSystem::UpdateFreezeables(float DeltaTime)
{
	for (int32& id : IceComponents)
	{
		//just started freezing
		if (CurrentlyFreezing.Contains(id) && !FreezeTimers.Contains(id))
		{
			FreezeTimers.Add(id, 0.0f);
			if (Freezables[id]->OnStartedFreezing.IsBound())
				Freezables[id]->OnStartedFreezing.Broadcast();
		}
		//just stopped freezing
		if (!CurrentlyFreezing.Contains(id) && FreezeTimers.Contains(id))
		{
			FreezeTimers.Remove(id);
			if (Freezables[id]->OnStoppedFreezing.IsBound())
				Freezables[id]->OnStoppedFreezing.Broadcast();
		}
		//Tick ignition
		if (FreezeTimers.Contains(id))
		{
			FreezeTimers[id] += DeltaTime;
			if (FreezeTimers[id] > Freezables[id]->TimeUntilFrozen)
			{
				Freezables[id]->IsFrozen = true;
				FreezeTimers.Remove(id);
				if (Freezables[id]->OnFrozen.IsBound())
					Freezables[id]->OnFrozen.Broadcast();
			}
		}
	}
}

void ABSElementComponentSystem::GetElementsAtPosition(const FVector& center, float radius, TArray<int32>& found) const
{
	const FBoxSphereBounds boxSphereBounds = GetBoxSphereFromRadius(center, radius);
	const FBoxCenterAndExtent inBoundingBoxQuery = FBoxCenterAndExtent(boxSphereBounds);
	if (DebugDrawGetAtLocation)
	{
		DrawDebugSphere(GetWorld(), center, boxSphereBounds.SphereRadius, 12, FColor::Red, false, 0, 0, 1);
		DrawDebugBox(GetWorld(), center, boxSphereBounds.BoxExtent, FColor::Red, false, 0, 0, 3);
	}
	found.Empty();
	for (FOrcTree::TConstElementBoxIterator<> OctreeIt(*OrcTree, inBoundingBoxQuery); OctreeIt.HasPendingElements(); OctreeIt.Advance())
		found.Add(OctreeIt.GetCurrentElement().Id);
}
FBoxSphereBounds ABSElementComponentSystem::GetBoxSphereFromRadius(const FVector& center, float radius) const
{
	const float sideLength = (2 * radius) / ROOT_3;
	const FVector extent = FVector(sideLength, sideLength, sideLength) * SPHERE_TO_BOX_FACTOR * 0.5f;
	return FBoxSphereBounds(center, extent, radius);
}
//Debug drawer.
void ABSElementComponentSystem::DrawOrcTree()
{
	int level = 0;
	float max;
	float offsetMax;
	float offset;
	FVector maxExtent;
	FVector center;
	FVector tempForCoercion;
	FBoxCenterAndExtent OldBounds = FBoxCenterAndExtent();

	int nodeCount = 0;
	int elementCount = 0;

	// Go through the nodes of the octree
	for (FOrcTree::TConstIterator<> NodeIt(*OrcTree); NodeIt.HasPendingNodes(); NodeIt.Advance())
	{
		const FOrcTree::FNode& CurrentNode = NodeIt.GetCurrentNode();
		const FOctreeNodeContext& CurrentContext = NodeIt.GetCurrentContext();
		const FBoxCenterAndExtent& CurrentBounds = CurrentContext.Bounds;

		nodeCount++;

		FOREACH_OCTREE_CHILD_NODE(ChildRef)
		{
			if (CurrentNode.HasChild(ChildRef))
				NodeIt.PushChild(ChildRef);
		}

		// If the extents have changed then we have moved a level.
		if (!OldBounds.Extent.Equals(CurrentBounds.Extent))
			level++;
		OldBounds = CurrentBounds;


		// Draw Node Bounds
		tempForCoercion = CurrentBounds.Extent;
		max = tempForCoercion.GetMax();
		center = CurrentBounds.Center;

		// To understand the math here check out the constructors in FOctreeNodeContext
		// Offset nodes that are not the root bounds
		if (!OrcTree->GetRootBounds().Extent.Equals(CurrentBounds.Extent))
		{
			for (int i = 1; i < level; i++)
			{
				// Calculate offset
				offsetMax = max / (1.0f + (1.0f / FOctreeNodeContext::LoosenessDenominator));
				offset = max - offsetMax;
				max = offsetMax;

				// Calculate Center Offset
				center.X += offset * ((center.X > 0) ? 1 : -1);
				center.Y += offset * ((center.Y > 0) ? 1 : -1);
				center.Z += offset * ((center.Z > 0) ? 1 : -1);
			}
		}
		maxExtent = FVector(max, max, max);

		DrawDebugBox(GetWorld(), center, maxExtent, FColor().Blue, false, 0.0f);
		DrawDebugSphere(GetWorld(), center + maxExtent, 4.0f, 12, FColor().Green, false, 0.0f);
		DrawDebugSphere(GetWorld(), center - maxExtent, 4.0f, 12, FColor().Red, false, 0.0f);

		for (FOrcTree::ElementConstIt ElementIt(CurrentNode.GetElementIt()); ElementIt; ++ElementIt)
		{
			const FOctTreeElement& Sample = *ElementIt;

			// Draw debug boxes around elements
			max = Sample.BoxSphereBounds.BoxExtent.GetMax();
			maxExtent = FVector(max, max, max);
			center = Sample.MyActor->GetActorLocation();

			DrawDebugBox(GetWorld(), center, maxExtent, FColor().Blue, false, 0.0f);
			DrawDebugSphere(GetWorld(), center + maxExtent, 4.0f, 12, FColor().White, false, 0.0f);
			DrawDebugSphere(GetWorld(), center - maxExtent, 4.0f, 12, FColor().White, false, 0.0f);
			elementCount++;
		}
	}
}

//Deprecated. Suat worked so hard on it :'(
/*
void AElementComponentSystem::UpdateBurnables(float DeltaTime)
{
	CurrentlyBurning.Empty();
	for (int32& id : BurnableComponents)
	{
		if (Burnables[id]->IsBurning)
			CurrentlyBurning.Add(id);
	}

	//Update distance to closest fire, for all burnables that are not allready on fire
	DistanceToFire.Empty();
	for (int32& burnableId : BurnableComponents)
	{
		UBSBurnableComponent* burnable = Burnables[burnableId];
		if (burnable->IsBurning)
			continue;

		int32 closestBurning = -1;
		float closestFire = BIG_DISTANCE;
		for (int32& burningId : CurrentlyBurning)
		{
			UBSBurnableComponent* burning = Burnables[burningId];
			const float distance = (burnable->GetOwner()->GetActorLocation() - burning->GetOwner()->GetActorLocation()).Size();
			if(distance < closestFire)
			{
				closestFire = distance;
				closestBurning = burning->ComponentID;
			}
		}
		DistanceWithId temp;
		temp.Id = closestBurning;
		temp.Distance = closestFire;
		DistanceToFire.Add(burnable->ComponentID, temp);
	}

	for (int32& burnableId : BurnableComponents)
	{
		//Check if burnable should start or stop igniting
		if (DistanceToFire.Contains(burnableId))
		{
			DistanceWithId IdDistance = DistanceToFire[burnableId];
			if (CurrentlyBurning.Contains(IdDistance.Id))
			{
				const float radius = Burnables[IdDistance.Id]->FireSpreadRadius;

				if (IdDistance.Distance < radius)
				{
					if (!IgnitionTimers.Contains(burnableId))
					{
						IgnitionTimers.Add(burnableId, 0.0f);
						if (Burnables[burnableId]->OnStartedIgnition.IsBound())
						{
							Burnables[burnableId]->OnStartedIgnition.Broadcast();
						}
					}
				}
				else if (IgnitionTimers.Contains(burnableId))
				{
					IgnitionTimers.Remove(burnableId);

					if (Burnables[burnableId]->OnStoppedIgnition.IsBound())
					{
						Burnables[burnableId]->OnStoppedIgnition.Broadcast();
					}
				}
			}
		}
		//Check if ignition should finish
		if (IgnitionTimers.Contains(burnableId))
		{
			IgnitionTimers[burnableId] += DeltaTime;
			if (IgnitionTimers[burnableId] >= Burnables[burnableId]->TimeUntilBurning)
			{
				if (Burnables[burnableId]->OnStartedBurning.IsBound())
				{
					Burnables[burnableId]->OnStartedBurning.Broadcast();
				}
				
				Burnables[burnableId]->IsBurning = true;
				IgnitionTimers.Remove(burnableId);
				if(!DeathTimers.Contains(burnableId))
					DeathTimers.Add(burnableId, 0.0f);
			}
		}
		if (DeathTimers.Contains(burnableId))
		{
			DeathTimers[burnableId] += DeltaTime;
			if (DeathTimers[burnableId] > Burnables[burnableId]->TimeUntilDeath)
			{
				if (Burnables[burnableId]->OnBurnedDown.IsBound())
				{
					Burnables[burnableId]->OnBurnedDown.Broadcast();
				}
				ComponentsToDestroy.Add(burnableId);
				
			}
		}
	}

	for (int32 id : ComponentsToDestroy)
		RemoveComponent(id);

}

void AElementComponentSystem::UpateExplosives(float DeltaTime)
{
	TArray<int32> Exploding;
	for (int32& id : ExplosiveComponents)
	{
		if(!Explosives[id]->IsActivated)
			continue;

		for (int32& burningId : CurrentlyBurning)
		{
			const float distance = (Explosives[id]->GetOwner()->GetActorLocation() - Burnables[burningId]->GetOwner()->GetActorLocation()).Size();
			if (distance < Burnables[burningId]->FireSpreadRadius)
				Explosives[id]->IsPrimed = true;
		}
		if(Explosives[id]->IsPrimed)
		{
			if (!PrimeTimers.Contains(id))
			{
				PrimeTimers.Add(id, 0.0f);
				if (Explosives[id]->OnFuseIgnition.IsBound())
					Explosives[id]->OnFuseIgnition.Broadcast();
			}
		}
		if (PrimeTimers.Contains(id))
		{
			PrimeTimers[id] += DeltaTime;
			if (PrimeTimers[id] >= Explosives[id]->FuseTime)
				Exploding.Add(id);
		}
	}

	for (int32& id : Exploding)
	{
		if(ComponentsToDestroy.Contains(id)) continue;
		ComponentsToDestroy.Add(id);

		if (!Explosives[id]->OnExplosion.IsBound())
			continue;

		FVector epicenter = Explosives[id]->GetOwner()->GetActorLocation();
		float radius = Explosives[id]->BlastRadius;
		float power = Explosives[id]->ExplosivePower;
		int32 explodingComponentsNum = 1;
		Explosives.ValueSort([epicenter](UBSExplosiveComponent& a, UBSExplosiveComponent& b)
		{
			const float aDistance = (a.GetOwner()->GetActorLocation() - epicenter).SizeSquared();
			const float bDistance = (b.GetOwner()->GetActorLocation() - epicenter).SizeSquared();
			return aDistance < bDistance;
		});
		for (auto pair : Explosives)
		{
			if(pair.Key == id) continue;
			const FVector otherLocation = pair.Value->GetOwner()->GetActorLocation();
			const FVector toOther = otherLocation - (epicenter / explodingComponentsNum);
			if (toOther.SizeSquared() > radius * radius) break;
			if (!pair.Value->IsActivated) continue;
			explodingComponentsNum++;
			epicenter += pair.Value->GetOwner()->GetActorLocation();
			radius += pair.Value->BlastRadius;
			power += pair.Value->ExplosivePower;
			ComponentsToDestroy.Add(pair.Key);
		}
		Explosives[id]->OnExplosion.Broadcast(epicenter / explodingComponentsNum, radius, power);
	}

	for (int32 id : ComponentsToDestroy)
		RemoveComponent(id);

}

*/
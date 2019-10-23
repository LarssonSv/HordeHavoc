#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericOctree.h"
#include "BSElementComponentSystem.generated.h"


//Octtree
USTRUCT()
struct FOctTreeElement
{
	GENERATED_BODY()
	int32 Id;
	FBoxSphereBounds BoxSphereBounds;
	AActor* MyActor;
};
struct FOctTreeSemantics
{
	enum { MaxElementsPerLeaf = 2 };
	enum { MinInclusiveElementsPerNode = 7 };
	enum { MaxNodeDepth = 12 };

	typedef TInlineAllocator<MaxElementsPerLeaf> ElementAllocator;

	FORCEINLINE static FBoxSphereBounds GetBoundingBox(const FOctTreeElement& Element)
	{
		return Element.BoxSphereBounds;
	}
	FORCEINLINE static bool AreElementsEqual(const FOctTreeElement& A, const FOctTreeElement& B)
	{
		return A.Id == B.Id;
	}
	FORCEINLINE static void SetElementId(const FOctTreeElement& Element, FOctreeElementId Id) { }
	FORCEINLINE static void ApplyOffset(FOctTreeElement& Element, FVector Offset)
	{
		FVector NewPostion = Element.MyActor->GetActorLocation() + Offset;
		Element.MyActor->SetActorLocation(NewPostion);
		Element.BoxSphereBounds.Origin = NewPostion;
	}
};
typedef TOctree<FOctTreeElement, FOctTreeSemantics> FOrcTree;

UCLASS()
class TEAM2_PROJECT2_API ABSElementComponentSystem : public AActor
{
	GENERATED_BODY()

public:	

	UPROPERTY(EditAnywhere)
	float Extent = 12000.f;
	UPROPERTY(EditDefaultsOnly)
	FRuntimeFloatCurve Curve;
	UPROPERTY(EditAnywhere)
	bool DebugDrawGetAtLocation = false;
	UPROPERTY(EditDefaultsOnly)
	bool DebugDrawEntireOrcTree = false;

private:

	int32 uidCounter = 0;
	FOrcTree* OrcTree = nullptr;
	TArray<int32> ComponentsToDestroy;
	TArray<FOctTreeElement*> elements;
	TMap<int32, class UBSElementComponentBase*> AllComponents;
	//Burnables
	TMap<int32, class UBSBurnableComponent*> Burnables;
	TArray<int32> BurnableComponents;
	TArray<int32> CurrentlyIgniting;
	TMap<int32, float> IgnitionTimers;
	TMap<int32, float> DeathTimers;
	TSet<int32> InfiniteFires;
	//Explosives
	TMap<int32, class UBSExplosiveComponent*> Explosives;
	TArray<int32> ExplosiveComponents;
	TMap<int32, float> PrimeTimers;
	TMap<int32, FVector> LocationsOfLastTrail;
	TMap<int32, int32> OrcsWalkedInto;
	TSet<int32> CheckedComponents;
	//Explodables
	TMap<int32, class UBSExplodableComponent*> Explodables;
	TArray<int32> ExplodableComponents;
	//Ice
	TMap<int32, class UBSIceComponent*> Freezables;
	TArray<int32> IceComponents;
	TArray<int32> CurrentlyFreezing;
	TMap<int32, float> FreezeTimers;


	//Constants
	const float ROOT_3 = 1.73205080757f;
	const float SPHERE_TO_BOX_FACTOR = 1.5f;
	const float MINIMUM_RADIUS = 25.f;

protected:
	virtual void BeginPlay() override;

public:	
	ABSElementComponentSystem();
	virtual void Tick(float DeltaTime) override;
	void AddElementComponent(class UBSElementComponentBase* component);
	void AddToComponentToDestroy(int32 id);
private:
	void RemoveElementComponent(int32 id);
	//Octree
	void UpdateTree();
	void DrawOrcTree();
	//functionality using octree
	UFUNCTION()
	void UpdateSpreading();
	void UpdateBurnables(float DeltaTime);
	void UpateExplosives(float DeltaTime);
	void UpdateFreezeables(float DeltaTime);
	void GetElementsAtPosition(const FVector& center, float extent, TArray<int32>& found) const;
	FBoxSphereBounds GetBoxSphereFromRadius(const FVector& center, float radius) const;
};

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSElementComponentBase.h"
#include "BSExplosiveComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM2_PROJECT2_API UBSExplosiveComponent : public UBSElementComponentBase
{
	GENERATED_BODY()

public:	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FExplosionDelegate, FVector, Epicenter, float, Radius, float, Power);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFuseIgnitionDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExplosionTrailDelegate, FVector, Location);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOilSpreadDelegate, int32, TimesUsed);

	/**Set if this component is activated or not.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsActivated = true;
	/**Set if the fuse is lit or not.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPrimed = false;
	/**How long shall the fuse burn until it explodes.*/
	UPROPERTY(EditDefaultsOnly)
	float FuseTime = 4.0f;
	/**How big of an explosion do you want to have. */
	UPROPERTY(EditDefaultsOnly)
	float BlastRadius = 300.f;
	/**The power of the explosive, added with any other explosives in the blast radius for a larger explosion*/
	UPROPERTY(EditDefaultsOnly)
	float ExplosivePower = 10;
	/**If the explosive should leave a trail or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LeavesBurnableTrail;
	/**The distance between dropped trail parts */
	UPROPERTY(EditDefaultsOnly)
	float DistanceBetweenTrailParts = 75.f;
	UPROPERTY(EditDefaultsOnly)
	float FirstDistance = 200.0f;

	/**The minimum factor to multiply with at the edge of the explosion.*/
	UPROPERTY(EditDefaultsOnly)
	float MinimumExplosiveFactor = 0.3f;
	/**Can this component spread oil*/
	UPROPERTY(EditDefaultsOnly)
	bool CanSpreadOil = false;
	/**If CanSpreadOil is true, how far will the oil spread*/
	UPROPERTY(EditDefaultsOnly)
	float OilSpreadRadius = 50.f;

	bool FirstTime = true;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 NumberOfSpreads = 3;
	UPROPERTY(BlueprintAssignable)
	FExplosionDelegate OnExplosion;
	UPROPERTY(BlueprintAssignable)
	FFuseIgnitionDelegate OnFuseIgnition;
	UPROPERTY(BlueprintAssignable)
	FExplosionTrailDelegate OnTrailUpdate;
	UPROPERTY(BlueprintAssignable)
	FFuseIgnitionDelegate OnActivated;
	UPROPERTY(BlueprintAssignable)
	FOilSpreadDelegate OnOilSpread;

	UBSExplosiveComponent();
};

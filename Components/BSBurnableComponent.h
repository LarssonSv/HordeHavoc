#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSElementComponentBase.h"
#include "BSBurnableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM2_PROJECT2_API UBSBurnableComponent : public UBSElementComponentBase
{
	GENERATED_BODY()

public:	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FIsBurningDelegate);

	/**Is object currently burning. Set to true if object should burn at start. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsBurning;
	/**Radius for how far the fire spreads. Set to 0 for not spreading the fire. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireSpreadRadius = 200.f;
	/**When on fire, this is the time until object is destroyed. Set to 0 for indestructible. */
	UPROPERTY(EditDefaultsOnly, Category = Timer)
	float TimeUntilDeath = 5.0f;
	/**When close to fire, this is the time until object catches fire. Set to 0 for inflamable. */
	UPROPERTY(EditDefaultsOnly, Category = Timer)
	float TimeUntilBurning = 1.0f;
	UPROPERTY(BlueprintAssignable)
	FIsBurningDelegate OnStartedBurning;
	UPROPERTY(BlueprintAssignable)
	FIsBurningDelegate OnStoppedBurning;
	UPROPERTY(BlueprintAssignable)
	FIsBurningDelegate OnBurnedDown;
	UPROPERTY(BlueprintAssignable)
	FIsBurningDelegate OnStartedIgnition;
	UPROPERTY(BlueprintAssignable)
	FIsBurningDelegate OnStoppedIgnition;


public:
	UBSBurnableComponent();
};

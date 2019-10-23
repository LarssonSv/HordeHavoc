#pragma once

#include "CoreMinimal.h"
#include "BSElementComponentBase.h"
#include "BSIceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM2_PROJECT2_API UBSIceComponent : public UBSElementComponentBase
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FIsFrozenDelegate);

	/**Can this component freeze other components.*/
	UPROPERTY(EditAnywhere)
	bool CanFreezeOthers = false;
	/**Is this component frozen. */
	UPROPERTY(EditAnywhere)
	bool IsFrozen = false;
	/**How long does it take to freeze this component.*/
	UPROPERTY(EditAnywhere)
	float TimeUntilFrozen = 5.0f;
	/**How far will this ice spread.*/
	UPROPERTY(EditAnywhere)
	float IceSpreadRadius = 50.0f;

	UPROPERTY(BlueprintAssignable)
	FIsFrozenDelegate OnFrozen;
	UPROPERTY(BlueprintAssignable)
	FIsFrozenDelegate OnStartedFreezing;
	UPROPERTY(BlueprintAssignable)
	FIsFrozenDelegate OnStoppedFreezing;


	UBSIceComponent();
};
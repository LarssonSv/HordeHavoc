#pragma once

#include "CoreMinimal.h"
#include "Components/BSElementComponentBase.h"
#include "BSExplodableComponent.generated.h"

UCLASS( ClassGroup = (Custom), meta = (BlueprintSpawnableComponent) )
class TEAM2_PROJECT2_API UBSExplodableComponent : public UBSElementComponentBase
{
	GENERATED_BODY()
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FIsExplodedDelegate, float, Power, FVector, DirectionFromExplosion);

	UPROPERTY(EditDefaultsOnly)
	int32 PowerRequired = 10;
	UPROPERTY(BlueprintAssignable)
	FIsExplodedDelegate OnExploded;
public:
	UBSExplodableComponent();
};

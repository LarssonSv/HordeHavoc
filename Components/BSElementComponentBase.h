#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BSElementComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAM2_PROJECT2_API UBSElementComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	int32 ComponentID = 0;
		
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void RemoveComponentFromSystem();
};

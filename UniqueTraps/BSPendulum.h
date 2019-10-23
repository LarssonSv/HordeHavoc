#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BSPendulum.generated.h"

UCLASS()
class TEAM2_PROJECT2_API ABSPendulum : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UBoxComponent* CollisionBox;
	UPROPERTY(EditAnywhere)
	float ArmLength = 200.0f;
	UPROPERTY(EditAnywhere)
	float Gravity = 1000.0f;

	UPROPERTY(BlueprintReadWrite)
	FVector Velocity;
	UPROPERTY(BlueprintReadWrite)
	FVector Origin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ApplyRotation = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartAngle = 90.0f;
private:
	int32 SMALL_VALUE = 3;
	FVector Right;
public:
	ABSPendulum();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};

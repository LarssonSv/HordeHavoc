// Fill out your copyright notice in the Description page of Project Settings.
//Author: Simon
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Components/BSProjectileComponent.h"
#include "Team2_Project2GameModeBase.h"
#include <BehaviorTree/BlackboardComponent.h>
#include <Engine/StaticMesh.h>
#include <Materials/MaterialInstanceDynamic.h>
#include "BSAIAgent.generated.h"

class AAIController;
class UCharacterMovementComponent;

USTRUCT(BlueprintType)
struct TEAM2_PROJECT2_API FWearableArmor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LocationOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator RotationOffset;
	


};

UCLASS()
class TEAM2_PROJECT2_API ABSAIAgent : public ACharacter
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathDelegate, EDeathType, death);
	

public:
	ABSAIAgent();
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category = "AIAgent")
	void MoveToLocation(const FVector& TargetLocation);
	UFUNCTION(BlueprintCallable, Category = "AIAgent")
	void Die(EDeathType death = EDeathType::Default);
	UFUNCTION(BlueprintCallable, Category = "AIAgent")
	bool GetCanMove() const;
	UFUNCTION(BlueprintCallable, Category = "AIAgent")
	void SetCanMove(bool x);
	UFUNCTION(BlueprintCallable, Category = "AIAgent")
	bool IsDead() const;
	/** If Duration is 0.0f, function wont set a timer */
	UFUNCTION(BlueprintCallable, Category = "AIAgent", meta = (ToolTip = "If duration is 0.0f, dont set a timer"))
	void SetMaxSpeed(float speedModifier, float duration);
	UFUNCTION(BlueprintCallable, Category = "AIAgent")
	void ResetMaxSpeed();

	//-----DO NOT CALL ON BEGIN PLAY-------
	UFUNCTION(BlueprintCallable, Category = "AIAgent")
	void DisableUnit();
	UFUNCTION(BlueprintCallable, Category = "AIAgent")
	void DisableUnitWithDelay();
	//-------------------------------------

	/*Only runs if Burning and not selected*/
	UFUNCTION(BlueprintCallable, Category = "AIAgent")
	void RunInCircleInit();
	UFUNCTION(BlueprintCallable, Category = "AIAgent")
	bool ActivateUnit();
	UFUNCTION(BlueprintCallable, Category = "AIAgent")
	bool IsUnitDisabled() const;
	UFUNCTION(BlueprintCallable, Category = "AIAgent")
	void Ragdoll();
	void StopMovement();
	UFUNCTION(BlueprintCallable, Category = "AIAgent")
	UBSProjectileComponent* GetProjectileComp();
	UFUNCTION(BlueprintCallable, Category = "AIAgent")
	UBSSelectableComponent* GetSelectableComp();


private:
	void SetWearables();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float size = 0.15f;
	UPROPERTY(EditDefaultsOnly)
	int32 Spacing = 50;
	UPROPERTY(EditDefaultsOnly)
	int32 Columns = 5;
	UPROPERTY(EditAnywhere)
	float MinOffset = 20.f;
	UPROPERTY(EditAnywhere)
	float MaxOffset = 100.f;
	UPROPERTY(BlueprintAssignable)
	FOnDeathDelegate OnDeath;
	UPROPERTY(EditAnywhere)
	float BurnPanicDistance = 400.0f;
	bool isDisabled = false;

	UStaticMeshComponent* HeadMesh;
	UStaticMeshComponent* WeaponMesh;
	UPROPERTY(EditDefaultsOnly)
	TArray<FWearableArmor> WearableHeadArmor;
	UPROPERTY(EditDefaultsOnly)
	TArray<FWearableArmor> EquipableWeapon;
protected:
	bool canMove = true;
	bool isDead = false;
	float currentAngle = 0.0f;



	UCharacterMovementComponent* CharMovementComp;
	AAIController* AiController;
	UBlackboardComponent* BlackBoard;
	FTimerHandle movementModifier;
	FTimerHandle delayedDisable;
	FTimerHandle circleTick;
};

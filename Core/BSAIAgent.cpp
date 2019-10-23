// Fill out your copyright notice in the Description page of Project Settings.
//Author: Simon
#include "BSAIAgent.h"
#include "NavigationSystem.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include <NavigationQueryFilter.h>
#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>
#include "Components/BSSelectableComponent.h"
#include <TimerManager.h>
#include "Components/BSElementComponentBase.h"
#include "Components/BSExplodableComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include <Engine/EngineTypes.h>
#include <GameFramework/Character.h>
#include "Components/BSBurnableComponent.h"

ABSAIAgent::ABSAIAgent()
{
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = ABSAIAgent::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	

	//Head = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HEAD"));
	
}

void ABSAIAgent::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<UActorComponent*> meshes = GetComponentsByClass(UStaticMeshComponent::StaticClass());
	const int32 size = meshes.Num();
	for (int32 i = 0; i < size; i++)
	{
		if(meshes[i]->GetName().Equals(TEXT("Head")))
			HeadMesh = Cast<UStaticMeshComponent>(meshes[i]);
		if(meshes[i]->GetName().Equals(TEXT("Weapon")))
			WeaponMesh = Cast<UStaticMeshComponent>(meshes[i]);
	}
	CharMovementComp = Cast<UCharacterMovementComponent>(GetComponentByClass(UCharacterMovementComponent::StaticClass()));
	AiController = Cast<AAIController>(GetController());
	if (AiController)
	{
		BlackBoard = AiController->GetBlackboardComponent();
	}

	//UMaterialInstanceDynamic * MISize0 = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), this);
	//UMaterialInstanceDynamic * MISize1 = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(1), this);

	//GetMesh()->SetMaterial(0, MISize0);
	//GetMesh()->SetMaterial(1, MISize1);

	//MISize0->SetScalarParameterValue(TEXT("Orc_scale"), 0.15);
	//MISize1->SetScalarParameterValue(TEXT("Orc_scale"), size);
	
	SetWearables();
}

void ABSAIAgent::MoveToLocation(const FVector& TargetLocation)
{
	if (!canMove)
		return;

	if (AiController && !BlackBoard)
	{
		BlackBoard = AiController->GetBlackboardComponent();
	}
	FVector LastVelocity = CharMovementComp->Velocity;
	if (CharMovementComp)
	{
		CharMovementComp->StopActiveMovement();
	}


	if (AiController)
	{
		if (CharMovementComp)
		{
			if (LastVelocity.Size() > 1.0f)
				CharMovementComp->bRequestedMoveUseAcceleration = false;
			else
				CharMovementComp->bRequestedMoveUseAcceleration = true;
		}

		if (BlackBoard)
			BlackBoard->SetValueAsVector(TEXT("MoveLocation"), TargetLocation);
	}

}

void ABSAIAgent::Die(EDeathType death)
{
	if (IsDead())
		return;

	Ragdoll();

	if (UBSSelectableComponent* SelectableComp = Cast<UBSSelectableComponent>(GetComponentByClass(UBSSelectableComponent::StaticClass())))
		SelectableComp->CanBeSelected = false;

	if (UCharacterMovementComponent* CharMovementComp = Cast<UCharacterMovementComponent>(GetComponentByClass(UCharacterMovementComponent::StaticClass())))
		CharMovementComp->StopActiveMovement();

	TArray<UActorComponent*> components = GetComponentsByClass(UBSElementComponentBase::StaticClass());
	for (UActorComponent* comp : components)
	{
		UBSExplodableComponent* explodable = Cast<UBSExplodableComponent>(comp);
		if (!explodable)
			continue;

		UBSElementComponentBase* element = Cast<UBSElementComponentBase>(comp);
		element->RemoveComponentFromSystem();
	}

	if (OnDeath.IsBound())
		OnDeath.Broadcast(death);

	SetCanMove(false);
	isDead = true;
}

void ABSAIAgent::Ragdoll()
{
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName(TEXT("Ragdoll")));
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetAnimInstanceClass(nullptr);
}

void ABSAIAgent::StopMovement()
{
	UCharacterMovementComponent* CharMovementComp = Cast<UCharacterMovementComponent>(GetComponentByClass(UCharacterMovementComponent::StaticClass()));

	if (CharMovementComp)
		CharMovementComp->StopActiveMovement();

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		UBlackboardComponent* blackBoard = AIController->GetBlackboardComponent();
		//blackBoard->SetValueAsVector(TEXT("MoveLocation"), GetActorLocation());
		blackBoard->ClearValue(TEXT("MoveLocation"));
	}
}

UBSProjectileComponent* ABSAIAgent::GetProjectileComp()
{
	if (UBSProjectileComponent* x = Cast<UBSProjectileComponent>(GetComponentByClass(UBSProjectileComponent::StaticClass())))
		return x;
	else
		return nullptr;
}


UBSSelectableComponent* ABSAIAgent::GetSelectableComp()
{
	if (UBSSelectableComponent* x = Cast<UBSSelectableComponent>(GetComponentByClass(UBSSelectableComponent::StaticClass())))
		return x;
	else
		return nullptr;
}

void ABSAIAgent::SetWearables()
{
	//Head
	const int32 randomArmorIndex = UKismetMathLibrary::RandomIntegerInRange(0, WearableHeadArmor.Num());
	if (randomArmorIndex < WearableHeadArmor.Num() && HeadMesh != nullptr && IsValid(HeadMesh))
	{
		HeadMesh->SetStaticMesh(WearableHeadArmor[randomArmorIndex].Mesh);
		HeadMesh->SetRelativeLocation(WearableHeadArmor[randomArmorIndex].LocationOffset);
		HeadMesh->SetRelativeRotation(WearableHeadArmor[randomArmorIndex].RotationOffset);
		HeadMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "HelmetAttachmentPoint");
	}
	//Right hand
	const int32 randomWeaponIndex = UKismetMathLibrary::RandomIntegerInRange(0, EquipableWeapon.Num());
	if (randomWeaponIndex < EquipableWeapon.Num() && WeaponMesh != nullptr && IsValid(WeaponMesh))
	{
		WeaponMesh->SetStaticMesh(EquipableWeapon[randomWeaponIndex].Mesh);
		WeaponMesh->SetRelativeLocation(EquipableWeapon[randomWeaponIndex].LocationOffset);
		WeaponMesh->SetRelativeRotation(EquipableWeapon[randomWeaponIndex].RotationOffset);
		WeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "RightHand");
	}
}

bool ABSAIAgent::GetCanMove() const
{
	return canMove;
}

void ABSAIAgent::SetCanMove(bool x)
{
	canMove = x;

	if (x == false)
	{
		if (UCharacterMovementComponent* CharMovementComp = Cast<UCharacterMovementComponent>(GetComponentByClass(UCharacterMovementComponent::StaticClass())))
		{
			CharMovementComp->StopActiveMovement();

			if (AAIController* AIController = Cast<AAIController>(GetController()))
			{
				UBlackboardComponent* blackBoard = AIController->GetBlackboardComponent();
				//blackBoard->SetValueAsVector(TEXT("MoveLocation"), GetActorLocation());
				blackBoard->ClearValue(TEXT("MoveLocation"));
			}
		}
	}

}

bool ABSAIAgent::IsDead() const
{
	return isDead;
}

void ABSAIAgent::SetMaxSpeed(float speedModifier, float duration)
{
	if (UCharacterMovementComponent* CharMovementComp = Cast<UCharacterMovementComponent>(GetComponentByClass(UCharacterMovementComponent::StaticClass())))
	{
		CharMovementComp->MaxWalkSpeed = 420.0f * speedModifier;
		if (duration > 0)
		{
			FTimerManager& TimerManager = GetWorldTimerManager();
			TimerManager.ClearTimer(movementModifier);
			TimerManager.SetTimer(movementModifier, this, &ABSAIAgent::ResetMaxSpeed, duration, false);
		}
		
	}
}

void ABSAIAgent::ResetMaxSpeed()
{
	if (UCharacterMovementComponent* CharMovementComp = Cast<UCharacterMovementComponent>(GetComponentByClass(UCharacterMovementComponent::StaticClass())))
		CharMovementComp->MaxWalkSpeed = 420.0f;
}


void ABSAIAgent::DisableUnit()
{
	if (isDisabled == true) return;
	
	isDisabled = true;
	SetCanMove(false);

	StopMovement();

	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		UBlackboardComponent* blackBoard = AIController->GetBlackboardComponent();
		blackBoard->ClearValue(TEXT("MoveLocation"));
	}

	if (UBSSelectableComponent* selectableComp = Cast<UBSSelectableComponent>(GetComponentByClass(UBSSelectableComponent::StaticClass())))
		selectableComp->Deactivate();
}

//Hack fix cuz UE4 timers cant handle arguments in methods
void ABSAIAgent::DisableUnitWithDelay()
{
	FTimerManager& TimerManager = GetWorldTimerManager();
	TimerManager.ClearTimer(delayedDisable);
	TimerManager.SetTimer(delayedDisable, this, &ABSAIAgent::DisableUnit, 0.3f, false);
}

void ABSAIAgent::PanicMode()
{

	UBSSelectableComponent* selectableComp = Cast<UBSSelectableComponent>(GetComponentByClass(UBSSelectableComponent::StaticClass()));
	UBSBurnableComponent* burnableComp = Cast<UBSBurnableComponent>(GetComponentByClass(UBSBurnableComponent::StaticClass()));

	if (selectableComp && burnableComp)
	{
		if (selectableComp->IsSelected || !burnableComp->IsBurning)
			return;
		
		currentAngle = FMath::RandRange(0.0f, 360.0f);

		FVector Location = GetActorLocation() + FVector((BurnPanicDistance * FMath::RandRange(minMaxFactor.x, minMaxFactor.y)) * FMath::Sin(FMath::DegreesToRadians(currentAngle)), BurnPanicDistance * FMath::Cos(FMath::DegreesToRadians(currentAngle)), 0);
		
		if (UCharacterMovementComponent* CharMovementComp = Cast<UCharacterMovementComponent>(GetComponentByClass(UCharacterMovementComponent::StaticClass())))
			CharMovementComp->bRequestedMoveUseAcceleration = false;

		MoveToLocation(Location);

		FTimerManager& TimerManager = GetWorldTimerManager();
		TimerManager.ClearTimer(circleTick);
		TimerManager.SetTimer(circleTick, this, &ABSAIAgent::PanicMode, 0.5f, false);

	}
}

bool ABSAIAgent::ActivateUnit()
{
	if (isDisabled == false) return false;

	SetCanMove(true);
	isDisabled = false;

	return true;
}

bool ABSAIAgent::IsUnitDisabled() const
{
	return isDisabled;
}

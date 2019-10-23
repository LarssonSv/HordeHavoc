#include "BSElementComponentBase.h"
#include "../Team2_Project2GameModeBase.h"
#include "BSElementComponentSystem.h"

void UBSElementComponentBase::BeginPlay()
{
	Super::BeginPlay();
	ATeam2_Project2GameModeBase* gameState = Cast<ATeam2_Project2GameModeBase>(GetWorld()->GetAuthGameMode());
	if(gameState != nullptr)
		gameState->GetElementComponentSystem()->AddElementComponent(this);
}

void UBSElementComponentBase::RemoveComponentFromSystem()
{
	ATeam2_Project2GameModeBase* gameState = Cast<ATeam2_Project2GameModeBase>(GetWorld()->GetAuthGameMode());
	if (gameState != nullptr)
		gameState->GetElementComponentSystem()->AddToComponentToDestroy(ComponentID);
}

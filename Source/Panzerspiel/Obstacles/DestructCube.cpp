// All rights reserved @Apfelstrudel Games.


#include "DestructCube.h"

#include "Panzerspiel/PanzerspielGameModeBase.h"

void ADestructCube::Destruct() {
	APanzerspielGameModeBase* GameMode = nullptr;
	if(UWorld* World = GetWorld())
		GameMode = Cast<APanzerspielGameModeBase>(World->GetAuthGameMode());
	if(GameMode)
		GameMode->RemoveWorldObstacle(this);

	Destroy();
}

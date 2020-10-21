// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "../CubeObstacle.h"
#include "DestructCube.generated.h"

/**
 * 
 */
UCLASS()
class PANZERSPIEL_API ADestructCube : public ACubeObstacle
{
	GENERATED_BODY()

	// Functions.
	public:
	UFUNCTION()
	void Destruct();
};

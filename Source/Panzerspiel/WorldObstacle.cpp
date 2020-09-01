// All rights reserved @Apfelstrudel Games.


#include "WorldObstacle.h"

// Sets default values
AWorldObstacle::AWorldObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComp"));
	SetRootComponent(RootSceneComp);
}

TArray<USceneComponent*> AWorldObstacle::GetCornerMarkers() {
	return CornerMarkers;
}

TArray<FObstacleEdge> AWorldObstacle::GetPossibleEdges(FVector2D BulletOrigin) {
	TArray<FObstacleEdge> Empty;
	return Empty;
}

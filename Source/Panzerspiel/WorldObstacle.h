// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldObstacle.generated.h"

USTRUCT()
struct PANZERSPIEL_API FObstacleEdge {

	GENERATED_USTRUCT_BODY()
public:
    FObstacleEdge(FVector2D NewOrigin = FVector2D::ZeroVector, FVector2D NewDirection = FVector2D::ZeroVector) {
		Origin = NewOrigin;
		Direction = NewDirection;
	}

	UPROPERTY()
	FVector2D Origin;

	UPROPERTY()
	FVector2D Direction;
};


UCLASS()
class PANZERSPIEL_API AWorldObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldObstacle();

protected:
	UPROPERTY(EditAnywhere, Category="Corners")
	TArray<USceneComponent*> CornerMarkers;

	UPROPERTY()
	USceneComponent *RootSceneComp;

	UFUNCTION()
	virtual TArray<USceneComponent*> GetCornerMarkers();

	UFUNCTION()
	virtual TArray<FObstacleEdge> GetPossibleEdges(FVector2D BulletOrigin);

};

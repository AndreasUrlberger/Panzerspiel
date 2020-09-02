// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "WorldObstacle.h"
#include "GameFramework/Actor.h"
#include "RicochetAimer.generated.h"

/**
 *	GetPossibleEdges()
 *	Time it takes to get all possible FObstacleEdges (shortest time multiple time hit.):
 *	3	Obstacles:	~ 0.006 - 0.007 ms	(All times while drawing the debug lines.)
 *	5	Obstacles:  ~ 0.010 - 0.014 ms
 *	10	Obstacles:  ~ 0.013 - 0.020 ms
 *	20	Obstacles:  ~ 0.020 - 0.040 ms
 *	50	Obstacles:  ~ 0.040 - 0.080 ms
 *	100	Obstacles:  ~ 0.070 - 0.140 ms  (Around 60 - 120 without DebugDraw.)
 *
 *	GetPossibleEdges()
 *	100 Obstacles: 	- 0.030 - 0.060 ms
 */
UCLASS()
class PANZERSPIEL_API ARicochetAimer : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, Category="CubeObstacles")
	TArray<class ACubeObstacle*> Cubes;

	UPROPERTY(EditAnywhere)
	class ATankPawn *TankPawn;
	
	UPROPERTY(EditAnywhere)
	class ATankPawn *TankPawn2;

	

	UPROPERTY(EditAnywhere, Category="Debug")
	float DisplayHeight = 100;

	UPROPERTY(VisibleAnywhere, Category="Debug")
	TArray<FObstacleEdge> Edges;
	
	UPROPERTY(VisibleAnywhere, Category="Debug")
	TArray<FObstacleEdge> IntersectedEdges;
	
	UPROPERTY(VisibleAnywhere, Category="Debug")
	TArray<FObstacleEdge> Edges1;
	
	UPROPERTY(VisibleAnywhere, Category="Debug")
	TArray<FObstacleEdge> Edges2;

	UPROPERTY(EditAnywhere, Category="Debug")
	float LineThickness = 5;

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugDraw = false;
	
	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugLog = false;

	
public:	
	// Sets default values for this actor's properties
	ARicochetAimer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ShowEdges(TArray<FObstacleEdge> EdgesToShow) const;
	
	UFUNCTION()
	static TArray<FObstacleEdge> IntersectArrays(TArray<FObstacleEdge> First, TArray<FObstacleEdge> Second);
};

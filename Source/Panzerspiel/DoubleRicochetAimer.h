// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"


#include "WorldObstacle.h"
#include "GameFramework/Actor.h"
#include "DoubleRicochetAimer.generated.h"

UCLASS()
class PANZERSPIEL_API ADoubleRicochetAimer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoubleRicochetAimer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<class ACubeObstacle*> Cubes;
	
	UPROPERTY(EditAnywhere)
	class ATankPawn *TankPawn;
	
	UPROPERTY(EditAnywhere)
	class ATankPawn *TankPawn2;

	UPROPERTY(EditAnywhere, Category="Debug")
	float DisplayHeight = 200;

	UPROPERTY(EditAnywhere, Category="Debug")
	float LineThickness = 15;
	
	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugDrawCombinations = false;
	
	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugDrawPaths = false;

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugLog = false;

	UPROPERTY(EditAnywhere, Category="Debug")
	int32 FirstEdgeToShow = 0;

	UPROPERTY(EditAnywhere, Category="Debug")
	int32 LastEdgeToShow = 0;
	
	UPROPERTY(EditAnywhere)
	float RaycastDistanceThreshold = 5;
	
	UPROPERTY(EditAnywhere)
	float RaycastHeight = 100;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void DrawEdge(const FObstacleEdge &Edge, const FColor Color) const;
	
	UFUNCTION()
    void DrawLine(const FVector2D &Start, const FVector2D &End, const FColor Color) const;

	UFUNCTION()
	static bool AreFacingAway(const FObstacleEdge &Edge1, const FObstacleEdge &Edge2, const FVector2D &Edge1Normal);
	
	UFUNCTION()
	static bool IsReflectionGonnaHit(const FObstacleEdge &ShooterEdge, const FObstacleEdge &TargetEdge, const FVector2D &ShooterEdgeNormal, const FVector2D &ShooterLocation, const FVector2D &ShootDirection);

	UFUNCTION()
	static FVector2D CalculateIntersect(const FVector2D &Edge1Start, const FVector2D &Edge1Dir, const FVector2D &Edge2Start, const FVector2D &Edge2Dir);

	UFUNCTION()
	bool HasLineOfSight(const FObstacleEdge& ShooterEdge, const FObstacleEdge& TargetEdge, const AActor *Shooter, const AActor *Target, const FVector2D &ShootDirection) const;
};


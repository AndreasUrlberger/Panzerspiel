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

	UPROPERTY(VisibleAnywhere, Category="Debug")
	TArray<FObstacleEdge> TargetEdges;
	
	UPROPERTY(VisibleAnywhere, Category="Debug")
	TArray<FObstacleEdge> ShooterEdges;

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugDrawCombinations = false;
	
	UPROPERTY(EditAnywhere, Category="Debug")
	int32 FirstEdgeToShow = 0;

	UPROPERTY(EditAnywhere, Category="Debug")
	int32 LastEdgeToShow = 0;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void DrawEdge(FObstacleEdge Edge, FColor Color) const;
	
	UFUNCTION()
    void DrawLine(FVector2D Start, FVector2D End, FColor Color) const;

	UFUNCTION()
	static FVector2D CalculateIntersect(const FVector2D Edge1Start, const FVector2D Edge1Dir, const FVector2D Edge2Start, const FVector2D Edge2Dir);
};

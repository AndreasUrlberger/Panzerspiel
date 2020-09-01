// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "WorldObstacle.h"
#include "CubeObstacle.generated.h"

/**
 * 
 */
UCLASS()
class PANZERSPIEL_API ACubeObstacle : public AWorldObstacle
{
	GENERATED_BODY()

	public:
	ACubeObstacle();

private:
	UPROPERTY(EditAnywhere, Category="Corners")
	USceneComponent *TopLeftC;
	
	UPROPERTY(EditAnywhere, Category="Corners")
	USceneComponent *TopRightC;
	
	UPROPERTY(EditAnywhere, Category="Corners")
	USceneComponent *BottomRightC;
	
	UPROPERTY(EditAnywhere, Category="Corners")
	USceneComponent *BottomLeftC;

	UPROPERTY(VisibleAnywhere, Category="Corners")
	TArray<FVector2D> CornersLocations;

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugLog = false;

protected:
	virtual void BeginPlay() override;

public:
	virtual TArray<FObstacleEdge> GetPossibleEdges(FVector2D BulletOrigin) override;	
};

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

private:
	UPROPERTY(EditAnywhere, Category="Corners")
	USceneComponent *TopLeftC;
	
	UPROPERTY(EditAnywhere, Category="Corners")
	USceneComponent *TopRightC;
	
	UPROPERTY(EditAnywhere, Category="Corners")
	USceneComponent *BottomRightC;
	
	UPROPERTY(EditAnywhere, Category="Corners")
	USceneComponent *BottomLeftC;

	UPROPERTY(VisibleInstanceOnly, Category="Debug")
	TArray<UObstacleEdge*> Edges;

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugLog = false;

protected:
	virtual void BeginPlay() override;

public:
	ACubeObstacle();

	virtual TArray<UObstacleEdge*> GetPossibleEdges(const FVector2D &BulletOrigin) const override;
};

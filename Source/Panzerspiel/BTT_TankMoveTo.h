// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_TankMoveTo.generated.h"

/**
 * 
 */
UCLASS()
class PANZERSPIEL_API UBTT_TankMoveTo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere)
	class AAITankPawn *TankPawn;

	UPROPERTY(VisibleAnywhere, Category="Navigation")
	TArray<FVector> PathPoints;

	UPROPERTY(VisibleAnywhere)
	class AActor *Enemy;

	UPROPERTY(VisibleAnywhere)
	class AAIController *AIController;

	UPROPERTY(EditAnywhere, Category="Navigation")
	float MinCurveRadius;

	UPROPERTY()
	UBehaviorTreeComponent *OwnerBTC;

	UPROPERTY(EditAnywhere, Category="Navigation")
	class USplineComponent *SplineComp;

	// Two points on the spline get merged if their distance falls below this threshold.
	UPROPERTY(EditAnywhere, Category="Navigation")
	float MergeThreshold = 15;

public:
	
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, ::uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, ::uint8* NodeMemory, float DeltaSeconds) override;

	UBTT_TankMoveTo();

	UFUNCTION()
	void Abort();

	UFUNCTION()
	void Finish();

private:
	// Return true if tank reached the end of the path.
	bool FollowPath(float DeltaTime);
	
	void LogArray(TArray<FVector> Array);

	// Creates new path points which include some new points to populate the SplineComp.
	UFUNCTION()
	void UpdatePathPoints();

	// Manipulates the PathPoints and populates the SplineComp, which gets manipulated further to smooth it out.
	UFUNCTION()
	void CreateSmoothSpline();
};
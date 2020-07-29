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

	UPROPERTY(VisibleAnywhere)
	TArray<FVector> PathPoints;

	UPROPERTY(VisibleAnywhere)
	class AActor *Enemy;

	UPROPERTY(VisibleAnywhere)
	class AAIController *AIController;

	UPROPERTY(EditAnywhere)
	float MinCurveRadius;

	UPROPERTY(VisibleAnywhere)
	class USplineComponent *SplineComp;

public:
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, ::uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, ::uint8* NodeMemory, float DeltaSeconds) override;

	UBTT_TankMoveTo();

private:
	// Return true if tank reached the end of the path.
	bool FollowPath(float DeltaTime);
	
	void LogArray(TArray<FVector> Array);

	void Abort(UBehaviorTreeComponent& OwnerComp);

	// Creates new path points which include some new points to populate the SplineComp.
	void UpdatePathPoints();
};
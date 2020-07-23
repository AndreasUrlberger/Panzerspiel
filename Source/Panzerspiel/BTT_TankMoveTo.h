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
	class ATankPawn *TankPawn;

	UPROPERTY(VisibleAnywhere)
	TArray<FVector> PathPoints;

	UPROPERTY(VisibleAnywhere)
	class AActor *Enemy;

	UPROPERTY(VisibleAnywhere)
	class AAIController *AIController;

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
};

// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_SimpleTankMoveTo.generated.h"

/**
* 
*/
UCLASS()
class PANZERSPIEL_API UBTTask_SimpleTankMoveTo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
	private:
	UPROPERTY()
	class ASimpleAITankPawn *AITankPawn;
	
	UPROPERTY()
	UBehaviorTreeComponent *OwnerBTC;

	UPROPERTY()
	class AActor *Enemy;

	UPROPERTY(EditAnywhere)
	float NavAgentRadius = 200;
	
	public:
	
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, ::uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, ::uint8* NodeMemory, float DeltaSeconds) override;

	UBTTask_SimpleTankMoveTo();

	UFUNCTION()
    void Abort();

	UFUNCTION()
    void Finish();

	private:	
	void LogArray(TArray<FVector> Array);
};
// All rights reserved @Apfelstrudel Games.


#include "BTT_TankMoveTo.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "NavigationPath.h"
#include "Containers/Array.h"
// Eventuell dem BuildFile "AIModule" und ggf. GameplayTasks" hinzufuegen.

void UBTT_TankMoveTo::OnGameplayTaskActivated(UGameplayTask& Task) {
    UE_LOG(LogTemp, Warning, TEXT("OnGameplayTaskActivated"));
}

EBTNodeResult::Type UBTT_TankMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, ::uint8* NodeMemory) {

    UE_LOG(LogTemp, Warning, TEXT("ExecuteTask called"));

    // Our TankAIController.
    AAIController *AIController = Cast<AAIController>(OwnerComp.GetAIOwner());
    check(!AIController);
    // Enemy from the Blackboard.
    AActor *Enemy = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>("Enemy"));
    if(Enemy) {
        FVector StartPos = AIController->GetNavAgentLocation();
        FVector EndPos = Enemy->GetActorLocation();
        UNavigationPath *NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(AIController, StartPos, EndPos, AIController);
        TArray<FVector> PathPoints = NavPath->PathPoints;
        FVector FirstPoint = PathPoints.Top();
        UE_LOG(LogTemp, Warning, TEXT("FirstPoint: %s"), *FirstPoint.ToString());
    }else {
        UE_LOG(LogTemp, Warning, TEXT("Something went wrong while trying to get the Enemy"));
    }
    return EBTNodeResult::Succeeded;
}

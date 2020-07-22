// All rights reserved @Apfelstrudel Games.


#include "BTT_TankMoveTo.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "NavigationPath.h"
#include "TankPawn.h"
#include "Containers/Array.h"
// Eventuell dem BuildFile "AIModule" und ggf. GameplayTasks" hinzufuegen.

void UBTT_TankMoveTo::OnGameplayTaskActivated(UGameplayTask& Task) {
    UE_LOG(LogTemp, Warning, TEXT("OnGameplayTaskActivated"));
}

EBTNodeResult::Type UBTT_TankMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, ::uint8* NodeMemory) {
    // Our TankAIController.
    AIController = Cast<AAIController>(OwnerComp.GetAIOwner());
    if(AIController) {
        // Enemy from the Blackboard.
        Enemy = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>("Enemy"));
        if(Enemy) {
            FVector StartPos = AIController->GetNavAgentLocation();
            FVector EndPos = Enemy->GetActorLocation();
            UNavigationPath *NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(AIController->GetPawn(), StartPos, EndPos);
            PathPoints = NavPath->PathPoints;
            FVector FirstPoint = PathPoints.Top();
            UE_LOG(LogTemp, Warning, TEXT("FirstPoint: %s"), *FirstPoint.ToString());
            TankPawn = Cast<ATankPawn>(AIController->GetPawn());
            if(TankPawn)
                TankPawn->MoveTo(FirstPoint);
        }else {
            UE_LOG(LogTemp, Warning, TEXT("Something went wrong while trying to get the Enemy"));
        }
    }else {
        UE_LOG(LogTemp, Warning, TEXT("Didnt get the AIController"));
    }
    return EBTNodeResult::InProgress;
}

void UBTT_TankMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, ::uint8* NodeMemory, float DeltaSeconds) {
    Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
    if(TankPawn)
        TankPawn->MoveTo(PathPoints.Top());
}

UBTT_TankMoveTo::UBTT_TankMoveTo() {
    bCreateNodeInstance = true;
    bNotifyTick = true;
}

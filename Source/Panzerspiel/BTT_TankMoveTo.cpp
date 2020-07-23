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
            UE_LOG(LogTemp, Warning, TEXT("NavPath is %s"), NavPath->IsValid() ? TEXT("valid") : TEXT("not valid"));
            LogArray(PathPoints);
            TankPawn = Cast<ATankPawn>(AIController->GetPawn());
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

    // Reached end of path thus this task has finished.
    if(FollowPath(DeltaSeconds))
        FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

UBTT_TankMoveTo::UBTT_TankMoveTo() {
    bCreateNodeInstance = true;
    bNotifyTick = true;
}

bool UBTT_TankMoveTo::FollowPath(float DeltaTime) {
    if(TankPawn) {
        if(TankPawn->MoveTo(PathPoints.Top(), DeltaTime)) {
            // Tank reached PathPoints.Top
            PathPoints.RemoveAt(0);
            if(PathPoints.Num() <= 0)
                return true;
        }
    }

    return false;
}


void UBTT_TankMoveTo::LogArray(TArray<FVector> Array) {
    UE_LOG(LogTemp, Warning, TEXT("PathPoints: "));
    for (int32 Index = 0; Index < Array.Num(); ++Index)
    {
        UE_LOG(LogTemp, Warning, TEXT("Point at %d: %s"), Index, *PathPoints[Index].ToString()); 
    }
}

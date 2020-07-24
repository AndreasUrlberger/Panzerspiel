// All rights reserved @Apfelstrudel Games.


#include "BTT_TankMoveTo.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "NavigationPath.h"
#include "TankCharacter.h"
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
            const FVector StartPos = AIController->GetNavAgentLocation();
            const FVector EndPos = Enemy->GetActorLocation();
            TankCharacter = Cast<ATankCharacter>(AIController->GetPawn());

            UNavigationPath *NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(AIController->GetPawn(), StartPos, EndPos, TankCharacter);
            PathPoints = NavPath->PathPoints;
            // Cant move.
            if(PathPoints.Num() <= 0)
                Abort(OwnerComp);
            UE_LOG(LogTemp, Warning, TEXT("NavPath is %s"), NavPath->IsValid() ? TEXT("valid") : TEXT("not valid"));
            LogArray(PathPoints);
        }else {
            Abort(OwnerComp);
        }
    }else {
        Abort(OwnerComp);
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
    if(IsValid(TankCharacter)) {
        if(TankCharacter->MoveTo(PathPoints[0], DeltaTime)) {
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

void UBTT_TankMoveTo::Abort(UBehaviorTreeComponent& OwnerComp) {
    TankCharacter = nullptr;
    FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}

// All rights reserved @Apfelstrudel Games.


#include "BTTask_SimpleTankMoveTo.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "NavigationPath.h"
#include "SimpleAITankPawn.h"
#include "Containers/Array.h"
#include "DrawDebugHelpers.h"

void UBTTask_SimpleTankMoveTo::OnGameplayTaskActivated(UGameplayTask& Task) {
	UE_LOG(LogTemp, Warning, TEXT("OnGameplayTaskActivated"));
}

EBTNodeResult::Type UBTTask_SimpleTankMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, ::uint8* NodeMemory) {
	// Needed for the Abort function.
	OwnerBTC = &OwnerComp;
	AITankPawn = Cast<ASimpleAITankPawn>(OwnerComp.GetAIOwner()->GetPawn());
	Enemy = Cast<ATankPawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
	if (AITankPawn && Enemy) {
		const FVector StartPos = AITankPawn->GetNavAgentLocation();
		const FVector EndPos = Enemy->GetActorLocation();
		//UE_LOG(LogTemp, Warning, TEXT("StartPos: %s, EndPos: %s"), *StartPos.ToCompactString(), *EndPos.ToCompactString());
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(AITankPawn, StartPos, EndPos, AITankPawn);
		const TArray<FVector> PathPoints = NavPath->PathPoints;
		if(!NavPath->IsValid())
			UE_LOG(LogTemp, Warning, TEXT("Path not valid"));
		if (PathPoints.Num() <= 0)
			Abort();
		else {
			//UE_LOG(LogTemp, Warning, TEXT("Path is %spartial"), NavPath->IsPartial()?TEXT(""):TEXT("not "));
			// Adds additional points since otherwise the tanks path would get too round and it would get stuck at corners a lot.
			LogArray(PathPoints);
			AITankPawn->FollowPathPoints(this, PathPoints);
		}
	} else {
		Abort();
	}
	return EBTNodeResult::InProgress;
}

void UBTTask_SimpleTankMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, ::uint8* NodeMemory, float DeltaSeconds) {
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

UBTTask_SimpleTankMoveTo::UBTTask_SimpleTankMoveTo() {
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

void UBTTask_SimpleTankMoveTo::LogArray(TArray<FVector> Array) {
	UE_LOG(LogTemp, Warning, TEXT("PathPoints: "));
	for (int32 Index = 0; Index < Array.Num(); ++Index) {
		UE_LOG(LogTemp, Warning, TEXT("Point at %d: %s"), Index, *Array[Index].ToString());
		if(Index < Array.Num() - 1)
			DrawDebugLine(GetWorld(), 	Array[Index], Array[Index + 1], FColor::Emerald, true);
	}
}

void UBTTask_SimpleTankMoveTo::Abort() {
	if(!Enemy)
		UE_LOG(LogTemp, Warning, TEXT("Enemy not set."));
	if(!AITankPawn)
		UE_LOG(LogTemp, Warning, TEXT("AITankPawn not set."));
	UE_LOG(LogTemp, Warning, TEXT("Task aborted."));
	AITankPawn = nullptr;
	FinishLatentTask(*OwnerBTC, EBTNodeResult::Failed);
}

void UBTTask_SimpleTankMoveTo::Finish() {
	FinishLatentTask(*OwnerBTC, EBTNodeResult::Succeeded);
}

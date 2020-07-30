// All rights reserved @Apfelstrudel Games.


#include "BTT_TankMoveTo.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "NavigationPath.h"
#include "AITankPawn.h"
#include "Containers/Array.h"
#include "Components/SplineComponent.h"

void UBTT_TankMoveTo::OnGameplayTaskActivated(UGameplayTask& Task) {
	UE_LOG(LogTemp, Warning, TEXT("OnGameplayTaskActivated"));
}

EBTNodeResult::Type UBTT_TankMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, ::uint8* NodeMemory) {
	// Needed for the Abort function.
	OwnerBTC = &OwnerComp;
	SplineComp->ClearSplinePoints(true);
	// Our TankAIController.
	AIController = Cast<AAIController>(OwnerComp.GetAIOwner());
	// Enemy from the Blackboard.
	Enemy = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValue<UBlackboardKeyType_Object>("Enemy"));
	if (AIController && Enemy) {
		const FVector StartPos = AIController->GetNavAgentLocation();
		const FVector EndPos = OwnerComp.GetBlackboardComponent()->GetValueAsVector("NewMoveLocation");//Enemy->GetActorLocation();
		TankPawn = Cast<AAITankPawn>(AIController->GetPawn());

		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
			AIController->GetPawn(), StartPos, EndPos, TankPawn);
		PathPoints = NavPath->PathPoints;
		// Cant move.
		if (PathPoints.Num() <= 0)
			Abort();
		UE_LOG(LogTemp, Warning, TEXT("NavPath is %s"), NavPath->IsValid() ? TEXT("valid") : TEXT("not valid"));
		LogArray(PathPoints);
		UpdatePathPoints();
		LogArray(PathPoints);
		TankPawn->FollowSpline(this, SplineComp, PathPoints);
	} else {
		Abort();
	}
	return EBTNodeResult::InProgress;
}

void UBTT_TankMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, ::uint8* NodeMemory, float DeltaSeconds) {
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// Reached end of path thus this task has finished.
	/*if (FollowPath(DeltaSeconds))
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);*/
}

UBTT_TankMoveTo::UBTT_TankMoveTo() {
	bCreateNodeInstance = true;
	bNotifyTick = true;
	SplineComp = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComp->SetDrawDebug(true);
	
}

bool UBTT_TankMoveTo::FollowPath(float DeltaTime) {
	if (IsValid(TankPawn)) {
		if (TankPawn->MoveTo(PathPoints[0], DeltaTime)) {
			// Tank reached PathPoints.Top
			PathPoints.RemoveAt(0);
			if (PathPoints.Num() <= 0)
				return true;
		}
	}

	return false;
}


void UBTT_TankMoveTo::LogArray(TArray<FVector> Array) {
	UE_LOG(LogTemp, Warning, TEXT("PathPoints: "));
	for (int32 Index = 0; Index < Array.Num(); ++Index) {
		UE_LOG(LogTemp, Warning, TEXT("Point at %d: %s"), Index, *Array[Index].ToString());
	}
}

void UBTT_TankMoveTo::Abort() {
	TankPawn = nullptr;
	FinishLatentTask(*OwnerBTC, EBTNodeResult::Failed);
}

void UBTT_TankMoveTo::Finish() {
	FinishLatentTask(*OwnerBTC, EBTNodeResult::Succeeded);
}

void UBTT_TankMoveTo::UpdatePathPoints() {
	TArray<FVector> NewPathPoints;
	for (int32 Index = 0; Index < PathPoints.Num(); ++Index) {
		PathPoints[Index].Z = 0;
	}
	// Add the first point (which is probably unnecessary unless we use the points for a spline).
	NewPathPoints.Add(PathPoints[0]);
	SplineComp->AddSplineWorldPoint(PathPoints[0]);
	for (int32 Index = 1; Index < PathPoints.Num() - 1; ++Index) {
		// Get points and distances we need. The Index calls are save since the loop starts at Index 1 and ends at Num() - 2.
		const FVector Point1 = PathPoints[Index - 1];
		const FVector Point2 = PathPoints[Index];
		const FVector Point3 = PathPoints[Index + 1];
		const FVector Distance1 = Point1 - Point2;
		const FVector Distance2 = Point3 - Point2;
		// Calculate new points.
		// First new point.
		// Comparing squared distances for a performance increase. The distance has to be at least twice as long as the
		// MinCurveRadius since otherwise the points from both sides could overlap. Its important to remember that we
		// have to multiply with 4 since we are comparing the squared distances thus 2 -> 4 (We could also put a *2
		// inside the FMath::Square brackets).
		// EDIT: Changed it to 3 times since before there could have been to points very close together in the middle.
		if (Distance1.SizeSquared() < 9*FMath::Square(MinCurveRadius)) {
			// We dont need to add another point since it would be on Point1 anyway.
		} else {
			// Add a point on the line from 2 to 1 in distance MinCurveRadius.
			const FVector NewPoint = Point2 + Distance1.GetSafeNormal() * MinCurveRadius;
			NewPathPoints.Add(NewPoint);
			SplineComp->AddSplineWorldPoint(NewPoint);
		}
		// Already existing point.
		NewPathPoints.Add(Point2);
		SplineComp->AddSplineWorldPoint(Point2);
		// Second new point.
		if (Distance2.SizeSquared() < 9*FMath::Square(MinCurveRadius)) {
			// We dont need to add another point since it would be on Point3 anyway.
		} else {
			const FVector NewPoint = Point2 + Distance2.GetSafeNormal() * MinCurveRadius;
			NewPathPoints.Add(NewPoint);
			SplineComp->AddSplineWorldPoint(NewPoint);
		}
	}
	// Add the last point.
	NewPathPoints.Add(PathPoints[PathPoints.Num() - 1]);
	SplineComp->AddSplineWorldPoint(PathPoints[PathPoints.Num() - 1]);
	// Finally store the NewPathPoints as our PathPoints.
	PathPoints = NewPathPoints;

	const int32 NumSplinePoints = SplineComp->GetNumberOfSplinePoints();
	const int32 SplineLength = SplineComp->GetSplineLength();
	UE_LOG(LogTemp, Warning, TEXT("SplinePoints: %d, SplineLength: %d"), NumSplinePoints, SplineLength);
}

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
	AITankPawn = Cast<AAITankPawn>(OwnerComp.GetAIOwner()->GetPawn());
	Enemy = Cast<ATankPawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey()));
	if (AITankPawn && Enemy) {
		const FVector StartPos = AITankPawn->GetNavAgentLocation();
		const FVector EndPos = Enemy->GetActorLocation();
		//UE_LOG(LogTemp, Warning, TEXT("StartPos: %s, EndPos: %s"), *StartPos.ToCompactString(), *EndPos.ToCompactString());
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(AITankPawn, StartPos, EndPos, AITankPawn);
		PathPoints = NavPath->PathPoints;
		if (PathPoints.Num() <= 0)
			Abort();
		else {
			//UE_LOG(LogTemp, Warning, TEXT("Path is %spartial"), NavPath->IsPartial()?TEXT(""):TEXT("not "));
			// Adds additional points since otherwise the tanks path would get too round and it would get stuck at corners a lot.
			LogArray(PathPoints);
			UpdatePathPoints();
			CreateSmoothSpline();
			LogArray(PathPoints);
			AITankPawn->FollowSpline(this, SplineComp, PathPoints);
		}
	} else {
		Abort();
	}
	return EBTNodeResult::InProgress;
}

void UBTT_TankMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, ::uint8* NodeMemory, float DeltaSeconds) {
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
}

UBTT_TankMoveTo::UBTT_TankMoveTo() {
	bCreateNodeInstance = true;
	bNotifyTick = true;
	SplineComp = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComp->SetDrawDebug(true);

}

void UBTT_TankMoveTo::LogArray(TArray<FVector> Array) {
	UE_LOG(LogTemp, Warning, TEXT("PathPoints: "));
	for (int32 Index = 0; Index < Array.Num(); ++Index) {
		UE_LOG(LogTemp, Warning, TEXT("Point at %d: %s"), Index, *Array[Index].ToString());
	}
}

void UBTT_TankMoveTo::Abort() {
	UE_LOG(LogTemp, Warning, TEXT("Task aborted due to missing Actors."));
	AITankPawn = nullptr;
	FinishLatentTask(*OwnerBTC, EBTNodeResult::Failed);
}

void UBTT_TankMoveTo::Finish() {
	FinishLatentTask(*OwnerBTC, EBTNodeResult::Succeeded);
}

void UBTT_TankMoveTo::UpdatePathPoints() {
	TArray<FVector> NewPathPoints;
	check(PathPoints.Num() > 0);
	for (int32 Index = 0; Index < PathPoints.Num(); ++Index) {
		PathPoints[Index].Z = 0;
	}
	// Add the first point (which is probably unnecessary unless we use the points for a spline).
	NewPathPoints.Add(PathPoints[0]);
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
		if (Distance1.SizeSquared() < 9 * FMath::Square(MinCurveRadius)) {
			// We dont need to add another point since it would be on Point1 anyway.
		} else {
			// Add a point on the line from 2 to 1 in distance MinCurveRadius.
			const FVector NewPoint = Point2 + Distance1.GetSafeNormal() * MinCurveRadius;
			NewPathPoints.Add(NewPoint);
		}
		// Already existing point.
		NewPathPoints.Add(Point2);
		// Second new point.
		if (Distance2.SizeSquared() < 9 * FMath::Square(MinCurveRadius)) {
			// We dont need to add another point since it would be on Point3 anyway.
		} else {
			const FVector NewPoint = Point2 + Distance2.GetSafeNormal() * MinCurveRadius;
			NewPathPoints.Add(NewPoint);
		}
	}
	// Add the last point.
	NewPathPoints.Add(PathPoints[PathPoints.Num() - 1]);
	// Finally store the NewPathPoints as our PathPoints.
	PathPoints = NewPathPoints;
}

void UBTT_TankMoveTo::CreateSmoothSpline() {
	// Important since a SplineComponent has two points whenever it gets created.
	SplineComp->ClearSplinePoints();
	// Merge two points if they are very close together (need to configure the threshold).
	// Is distance to next point under MinCurveRadius?
	// Yes{
	// 		Scale OutTangent down to a specific small value (maybe distance between them).
	// 		Scale InTangent of the second point down as well.
	// 		(InTangent and outTangent are the same if we dont specify otherwise.)
	//	  }

	// DistanceMerge.
	for (int32 Index = 0; Index < PathPoints.Num() - 1; ++Index) {
		const float SquaredDistanceNext = (PathPoints[Index + 1] - PathPoints[Index]).SizeSquared();
		if (SquaredDistanceNext < FMath::Square(MergeThreshold)) {
			// Calculate the point in the middle.
			// Might as well calculate the median tangent to smooth the curve a little bit.
			const FVector NewPoint = PathPoints[Index] + (PathPoints[Index + 1] - PathPoints[Index]) / 2;
			PathPoints[Index] = NewPoint;
			PathPoints.RemoveAt(Index + 1);
		}
	}

	// Populate SplineComponent.
	for (int32 Index = 0; Index < PathPoints.Num(); ++Index) {
		const FVector Point = PathPoints[Index];
		SplineComp->AddSplinePoint(Point, ESplineCoordinateSpace::World, true);
		SplineComp->SetSplinePointType(Index, ESplinePointType::CurveCustomTangent, true);
		SplineComp->SetUpVectorAtSplinePoint(Index, FVector::UpVector, ESplineCoordinateSpace::World, true);
	}

	// TangentDownScaling.
	// Last point doesnt get scaled.
	for (int32 Index = 0; Index < PathPoints.Num() - 1; ++Index) {
		const float SquaredDistanceNext = (PathPoints[Index + 1] - PathPoints[Index]).SizeSquared();
		const float SquaredTangentSize = SplineComp->GetTangentAtSplinePoint(Index, ESplineCoordinateSpace::World).SizeSquared();
		if (SquaredTangentSize > SquaredDistanceNext) {
			FVector NewTangent = SplineComp->GetTangentAtSplinePoint(Index, ESplineCoordinateSpace::World) *
				FMath::Sqrt(SquaredDistanceNext / SquaredTangentSize);
			SplineComp->SetTangentsAtSplinePoint(Index, NewTangent, NewTangent, ESplineCoordinateSpace::World, true);
		}
	}
	SplineComp->UpdateSpline();
}

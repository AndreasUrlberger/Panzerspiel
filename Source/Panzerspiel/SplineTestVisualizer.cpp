// All rights reserved @Apfelstrudel Games.


#include "SplineTestVisualizer.h"
#include "Components/SplineComponent.h"
#include "AITankPawn.h"

// Sets default values
ASplineTestVisualizer::ASplineTestVisualizer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SplineComp = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComp->SetDrawDebug(true);

	SplineComp->ClearSplinePoints();
	SplineComp->AddSplinePoint(FVector(600, 600, 200), ESplineCoordinateSpace::World, true);
}

// Called when the game starts or when spawned
void ASplineTestVisualizer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASplineTestVisualizer::Tick(float DeltaTime)
{
	if(TankPawn) {
		if(TankPawn->PathPoints.Num() > 0) {
			SplineComp->ClearSplinePoints();
			TArray<FVector> Points = TankPawn->PathPoints;
			for(int32 Index = 0; Index < Points.Num(); ++Index) {
				FVector Point = Points[Index];
				Point.Z = 10;
				SplineComp->AddSplinePoint(Point, ESplineCoordinateSpace::World, false);
				SplineComp->SetSplinePointType(Index, ESplinePointType::Curve, true);
				SplineComp->SetUpVectorAtSplinePoint(Index, FVector::UpVector, ESplineCoordinateSpace::World, true);
				// Merge two points if they are very close together (need to configure the threshold).
				// Is distance to next point under MinCurveRadius?
				// Yes{
				// 		Scale OutTangent down to a specific small value (maybe distance between them).
				// 		Scale InTangent of the second point down as well.
				//	  }
				
				
			}
			SplineComp->UpdateSpline();
		}
	}
	Super::Tick(DeltaTime);
	if(AddPoint) {
		const FVector RandomLocation = FVector(FMath::RandRange(-1000, 1000), FMath::RandRange(-1000, 1000), 200);
		SplineComp->AddSplineWorldPoint(RandomLocation);
		AddPoint = false;
	}
}


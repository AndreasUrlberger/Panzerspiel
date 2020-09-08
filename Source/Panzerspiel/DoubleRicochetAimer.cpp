// All rights reserved @Apfelstrudel Games.


#include "DoubleRicochetAimer.h"
#include "CubeObstacle.h"
#include "Kismet/GameplayStatics.h"
#include "TankPawn.h"
#include "RicochetAimer.h"
#include "DrawDebugHelpers.h"

// Sets default values
ADoubleRicochetAimer::ADoubleRicochetAimer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Cubes.Empty();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, ACubeObstacle::StaticClass(), FoundActors);
	for (AActor* Cube : FoundActors)
		Cubes.Add(Cast<ACubeObstacle>(Cube));
}

// Called when the game starts or when spawned
void ADoubleRicochetAimer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoubleRicochetAimer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Debug
	const double Start = FPlatformTime::Seconds();
	TargetEdges.Empty();
	ShooterEdges.Empty();
	int32 FoundCounter = 0;
	int32 FoundCounter2 = 0;
	
	if(!(TankPawn && TankPawn2))
		return;
	// Get some variables that we'll need.
	const FVector2D ShooterLocation = FVector2D(TankPawn->GetActorLocation().X, TankPawn->GetActorLocation().Y);
	const FVector2D TargetLocation = FVector2D(TankPawn2->GetActorLocation().X, TankPawn2->GetActorLocation().Y);

	// Get all edges that are visible from the shooter and all that are visible from the target.
	/*TArray<FObstacleEdge> ShooterEdges;
	TArray<FObstacleEdge> TargetEdges;*/
	for(ACubeObstacle *Cube : Cubes) {
		ShooterEdges.Append(Cube->GetPossibleEdges2(ShooterLocation));
		TargetEdges.Append(Cube->GetPossibleEdges2(TargetLocation));
	}
	UE_LOG(LogTemp, Warning, TEXT("Max. possible combinations: %d"), TargetEdges.Num() * ShooterEdges.Num());

	for(FObstacleEdge TargetEdge : TargetEdges) {
		// Check whether the other edge can be seen by this one (not implemented yet).
		const FVector2D MirroredTarget = ARicochetAimer::MirrorPoint(TargetLocation, TargetEdge.Start, TargetEdge.End - TargetEdge.Start);
		for(FObstacleEdge ShooterEdge : ShooterEdges) {
			const FVector2D TwiceMirroredTarget = ARicochetAimer::MirrorPoint(MirroredTarget, ShooterEdge.Start, ShooterEdge.End - ShooterEdge.Start);
			const FVector2D ShootDirection = TwiceMirroredTarget - ShooterLocation;
			const FVector2D ShootersEdgeStart = ShooterEdge.Start - ShooterLocation;
			const FVector2D ShootersEdgeEnd = ShooterEdge.End - ShooterLocation;
			// Check whether ShootDirection lies between ShootersEdgeStart and ShootersEdgeEnd.
			if((ShootDirection ^ ShootersEdgeStart) * (ShootDirection ^ ShootersEdgeEnd) < 0) {
				const FVector2D TargetEdgeNormal = FVector2D(TargetEdge.End.Y - TargetEdge.Start.Y, -(TargetEdge.End.X - TargetEdge.Start.X));
				const FVector2D ShooterEdgeNormal = FVector2D(ShooterEdge.End.Y - ShooterEdge.Start.Y, -(ShooterEdge.End.X - ShooterEdge.Start.X));
				const FVector2D MirroredShootDirection = ARicochetAimer::MirrorVector(ShootDirection, ShooterEdge.Start, ShooterEdgeNormal);
				if((MirroredShootDirection | TargetEdgeNormal) > 0) {
					// This is a possible edge combination.
					++FoundCounter;
					FVector2D TargetEdgeMiddle = TargetEdge.Start + (TargetEdge.End - TargetEdge.Start)/2;
					FVector2D ShooterEdgeMiddle = ShooterEdge.Start + (ShooterEdge.End - ShooterEdge.Start)/2;
					FVector2D ShooterToTargetEdge = TargetEdgeMiddle - ShooterEdgeMiddle;
					if((TargetEdgeNormal | ShooterToTargetEdge) < 0) {
						const FVector2D ShooterEdgeIntersect = CalculateIntersect(ShooterEdge.Start, ShooterEdge.End - ShooterEdge.Start, ShooterLocation, ShootDirection);
						const FVector2D ShooterMirroredShootDirection = ARicochetAimer::MirrorPoint(ShooterLocation, ShooterEdgeIntersect, ShooterEdgeNormal) - ShooterEdgeIntersect;
						const FVector2D StartDirection = TargetEdge.Start - ShooterEdgeIntersect;
						const FVector2D EndDirection = TargetEdge.End - ShooterEdgeIntersect;
						if((ShooterMirroredShootDirection ^ StartDirection) * (ShooterMirroredShootDirection ^ EndDirection) < 0) {
							// DebugHelpers.
							++FoundCounter2;
							/*if(FoundCounter >= FirstEdgeToShow && FoundCounter < LastEdgeToShow) {
								if(bDebugDrawCombinations) DrawEdge(TargetEdge, FColor::Red);
								if(bDebugDrawCombinations) DrawEdge(ShooterEdge, FColor::Green);
								if(bDebugDrawCombinations) DrawLine(ShooterLocation, ShooterEdgeIntersect, FColor::Orange);
								if(bDebugDrawCombinations) DrawLine(ShooterEdgeIntersect, ShooterEdgeIntersect + 100 * ShooterMirroredShootDirection, FColor::Orange);
								if(bDebugDrawCombinations) DrawLine(ShooterEdgeIntersect, ShooterEdgeIntersect + 100 * StartDirection, FColor::Blue);
								if(bDebugDrawCombinations) DrawLine(ShooterEdgeIntersect, ShooterEdgeIntersect + 100 * EndDirection, FColor::Purple);
							}*/
						}
					}
				}
			}
		}
	}

	const double End = FPlatformTime::Seconds();
	UE_LOG(LogTemp, Warning, TEXT("code executed in %f seconds, found %d edges, then %d edges."), End-Start, FoundCounter, FoundCounter2);
}

void ADoubleRicochetAimer::DrawEdge(const FObstacleEdge Edge, const FColor Color) const {
	const FVector From = FVector(Edge.Start.X, Edge.Start.Y, DisplayHeight);
	const FVector To = FVector(Edge.End.X, Edge.End.Y, DisplayHeight);
	DrawDebugLine(GetWorld(), From, To, Color, false, -1, 0, LineThickness);
}

void ADoubleRicochetAimer::DrawLine(FVector2D Start, FVector2D End, FColor Color) const {
	DrawDebugLine(GetWorld(), FVector(Start.X, Start.Y, DisplayHeight), FVector(End.X, End.Y, DisplayHeight), Color, false, -1, 0, LineThickness);
}

FVector2D ADoubleRicochetAimer::CalculateIntersect(const FVector2D Edge1Start, const FVector2D Edge1Dir,
                                                   const FVector2D Edge2Start, const FVector2D Edge2Dir) {

	const FVector2D OriginNew = Edge1Start - Edge2Start;
	const FVector2D Pi = Edge1Dir;
	const FVector2D Theta = Edge2Dir;
	
	const float PiD = Edge1Dir.X - Edge1Dir.Y;
	const float ThetaD = Edge2Dir.X - Edge2Dir.Y;
	const float UrD = OriginNew.Y - OriginNew.X;

	const float InsetNumbers = Pi.X * UrD/PiD;
	const float InsetTheta = Pi.X * ThetaD/PiD - Theta.X;
	const float ThetaErg = (OriginNew.X + InsetNumbers) / -InsetTheta;

	return Edge2Start + ThetaErg * Edge2Dir;
}


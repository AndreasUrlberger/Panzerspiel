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
					// Check whether the TargetEdge and the ShooterEdge are not facing away from each other and thus can reflect a bullet properly.
					if(AreFacingAway(TargetEdge, ShooterEdge, TargetEdgeNormal))
						continue;
					if(!IsReflectionGonnaHit(ShooterEdge, TargetEdge, ShooterEdgeNormal, ShooterLocation, ShootDirection))
						continue;
					// If we reach this point this is a possible edge combination.
					++FoundCounter;
				}
			}
		}
	}

	const double End = FPlatformTime::Seconds();
	UE_LOG(LogTemp, Warning, TEXT("code executed in %f seconds, found %d edges."), End-Start, FoundCounter);
}

void ADoubleRicochetAimer::DrawEdge(const FObstacleEdge &Edge, const FColor Color) const {
	const FVector From = FVector(Edge.Start.X, Edge.Start.Y, DisplayHeight);
	const FVector To = FVector(Edge.End.X, Edge.End.Y, DisplayHeight);
	DrawDebugLine(GetWorld(), From, To, Color, false, -1, 0, LineThickness);
}

void ADoubleRicochetAimer::DrawLine(const FVector2D &Start, const FVector2D &End, const FColor Color) const {
	DrawDebugLine(GetWorld(), FVector(Start.X, Start.Y, DisplayHeight), FVector(End.X, End.Y, DisplayHeight), Color, false, -1, 0, LineThickness);
}

// Return true if there is no way a bullet reflected from one edge could ever hit the other edge.
bool ADoubleRicochetAimer::AreFacingAway(const FObstacleEdge &Edge1, const FObstacleEdge &Edge2, const FVector2D &Edge1Normal) {
	const FVector2D Edge1Middle = Edge1.Start + (Edge1.End - Edge1.Start)/2;
	const FVector2D Edge2Middle = Edge2.Start + (Edge2.End - Edge2.Start)/2;
	const FVector2D Edge2ToEdge1Direction = Edge1Middle - Edge2Middle;
	return (Edge1Normal | Edge2ToEdge1Direction) >= 0;
}

// Tells whether the bullet can, once reflected at the edge, hit the other edge, judging by direction and origin only.
bool ADoubleRicochetAimer::IsReflectionGonnaHit(const FObstacleEdge &ShooterEdge, const FObstacleEdge &TargetEdge, const FVector2D &ShooterEdgeNormal, const FVector2D &ShooterLocation, const FVector2D &ShootDirection) {
	const FVector2D ShooterEdgeIntersect = CalculateIntersect(ShooterEdge.Start, ShooterEdge.End - ShooterEdge.Start, ShooterLocation, ShootDirection);
	const FVector2D ShooterMirroredShootDirection = ARicochetAimer::MirrorPoint(ShooterLocation, ShooterEdgeIntersect, ShooterEdgeNormal) - ShooterEdgeIntersect;
	const FVector2D StartDirection = TargetEdge.Start - ShooterEdgeIntersect;
	const FVector2D EndDirection = TargetEdge.End - ShooterEdgeIntersect;
	return (ShooterMirroredShootDirection ^ StartDirection) * (ShooterMirroredShootDirection ^ EndDirection) < 0;
}

FVector2D ADoubleRicochetAimer::CalculateIntersect(const FVector2D &Edge1Start, const FVector2D &Edge1Dir,
                                                   const FVector2D &Edge2Start, const FVector2D &Edge2Dir) {

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


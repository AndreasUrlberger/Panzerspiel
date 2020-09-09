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
	int32 FoundCounter = 0;
	
	if(!(TankPawn && TankPawn2))
		return;
	// Get some variables that we'll need.
	const FVector2D ShooterLocation = FVector2D(TankPawn->GetActorLocation());
	const FVector2D TargetLocation = FVector2D(TankPawn2->GetActorLocation());

	// Get all edges that are visible from the shooter and all that are visible from the target.
	TArray<FObstacleEdge> ShooterEdges;
	TArray<FObstacleEdge> TargetEdges;
	for(ACubeObstacle *Cube : Cubes) {
		ShooterEdges.Append(Cube->GetPossibleEdges2(ShooterLocation));
		TargetEdges.Append(Cube->GetPossibleEdges2(TargetLocation));
	}
	
	for(FObstacleEdge &TargetEdge : TargetEdges) {
		// Check whether the other edge can be seen by this one (kinda implemented in a later step).
		const FVector2D MirroredTarget = ARicochetAimer::MirrorPoint(TargetLocation, TargetEdge.Start, TargetEdge.End - TargetEdge.Start);
		for(FObstacleEdge &ShooterEdge : ShooterEdges) {
			const FVector2D TwiceMirroredTarget = ARicochetAimer::MirrorPoint(MirroredTarget, ShooterEdge.Start, ShooterEdge.End - ShooterEdge.Start);
			const FVector2D ShootDirection = TwiceMirroredTarget - ShooterLocation;
			// Interestingly this test is not obsolete by to the following test. This is the case since it does not
			// detect when the shooting direction points in the exact opposite.
			const FVector2D ShooterEdgeMiddle = ShooterEdge.Start + (ShooterEdge.End - ShooterEdge.Start)/2;
			if((ShootDirection | (ShooterEdgeMiddle - ShooterLocation)) < 0)
				continue;
			const FVector2D ShootersEdgeStart = ShooterEdge.Start - ShooterLocation;
			const FVector2D ShootersEdgeEnd = ShooterEdge.End - ShooterLocation;
			// Check whether ShootDirection lies between ShootersEdgeStart and ShootersEdgeEnd.
			if((ShootDirection ^ ShootersEdgeStart) * (ShootDirection ^ ShootersEdgeEnd) >= 0)
				continue;
			const FVector2D TargetEdgeNormal = FVector2D(TargetEdge.End.Y - TargetEdge.Start.Y, -(TargetEdge.End.X - TargetEdge.Start.X));
			const FVector2D ShooterEdgeNormal = FVector2D(ShooterEdge.End.Y - ShooterEdge.Start.Y, -(ShooterEdge.End.X - ShooterEdge.Start.X));
			const FVector2D MirroredShootDirection = ARicochetAimer::MirrorVector(ShootDirection, ShooterEdge.Start, ShooterEdgeNormal);
			if((MirroredShootDirection | TargetEdgeNormal) <= 0)
				continue;
			// Check whether the TargetEdge and the ShooterEdge are not facing away from each other and thus can reflect a bullet properly.
			if(AreFacingAway(TargetEdge, ShooterEdge, TargetEdgeNormal))
				continue;
			if(!IsReflectionGonnaHit(ShooterEdge, TargetEdge, ShooterEdgeNormal, ShooterLocation, ShootDirection))
				continue;
			// If we reach this point this is a possible edge combination.
			if(!HasLineOfSight(ShooterEdge, TargetEdge, TankPawn, TankPawn2, ShootDirection))
				continue;
			++FoundCounter;
			if(FoundCounter >= FirstEdgeToShow && FoundCounter <= LastEdgeToShow) {
				if(bDebugDrawCombinations) DrawEdge(TargetEdge, FColor::Red);
				if(bDebugDrawCombinations) DrawEdge(ShooterEdge, FColor::Green);
			}
		}
	}

	const double End = FPlatformTime::Seconds();
	if(bDebugLog) UE_LOG(LogTemp, Warning, TEXT("code executed in %f seconds, found %d edges in %d possible."), End-Start, FoundCounter, TargetEdges.Num() * ShooterEdges.Num());
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

bool ADoubleRicochetAimer::HasLineOfSight(const FObstacleEdge& ShooterEdge, const FObstacleEdge& TargetEdge,
	const AActor *Shooter, const AActor *Target, const FVector2D &ShootDirection) const {
	UWorld *World = GetWorld();
	if(!World)
		return false;
	// Setup first raycast (Shooter -> ShooterEdge).
	FVector From = Shooter->GetActorLocation();
	FVector2D ShooterLocation = FVector2D(From.X, From.Y);
	// TODO: These calculations are already done in IsReflectionGonnaHit, it might be better to safe them somewhere for later use.
	const FVector2D ShooterEdgeIntersect = CalculateIntersect(ShooterEdge.Start, ShooterEdge.End - ShooterEdge.Start, ShooterLocation, ShootDirection);
	FHitResult HitResult;
	From.Z = RaycastHeight;
	FVector To = FVector(ShooterEdgeIntersect.X, ShooterEdgeIntersect.Y, RaycastHeight);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Shooter);
	// TODO: I guess another collision channel could be less expensive.
	// We do the trace further than we might need to make sure we'll definitely hit the edge.
	World->LineTraceSingleByChannel(HitResult, From, From + 2 * (To - From), ECollisionChannel::ECC_Camera, Params);
	// Value the first raycast, we return false if the ray hit to far away from the supposed hit but we give it a small
	// threshold since raycasts are not that precise and we're using floats.
	if(FVector::DistSquaredXY(To, HitResult.Location) > RaycastDistanceThreshold)
		return false;
	//Debug:
	FObstacleEdge HitBackup1 = FObstacleEdge(FVector2D(From), FVector2D(HitResult.Location));

	// Setup second raycast (ShooterEdge -> TargetEdge).
	Params.ClearIgnoredActors();
	Params.AddIgnoredActor(ShooterEdge.Parent);
	From = FVector(ShooterEdgeIntersect.X, ShooterEdgeIntersect.Y, RaycastHeight);
	FVector2D ShooterEdgeNormal = FVector2D(ShooterEdge.End.Y - ShooterEdge.Start.Y, -(ShooterEdge.End.X - ShooterEdge.Start.X));
	FVector2D MirroredShootDirection = ARicochetAimer::MirrorVector(ShootDirection, ShooterEdgeIntersect, ShooterEdgeNormal);
	const FVector2D TargetEdgeIntersect = CalculateIntersect(TargetEdge.Start, TargetEdge.End - TargetEdge.Start, ShooterEdgeIntersect, MirroredShootDirection);
	To = FVector(TargetEdgeIntersect.X, TargetEdgeIntersect.Y, RaycastHeight);
	World->LineTraceSingleByChannel(HitResult, From, From + 2 * (To - From), ECollisionChannel::ECC_Camera, Params);
	// Value second raycast.
	if(FVector::DistSquaredXY(To, HitResult.Location) > RaycastDistanceThreshold)
		return false;
	// Debug: 
	FObstacleEdge HitBackup2 = FObstacleEdge(FVector2D(From), FVector2D(HitResult.Location));

	// Setup third raycast (Target -> TargetEdge).
	Params.ClearIgnoredActors();
	Params.AddIgnoredActor(Target);
	From = Target->GetActorLocation();
	From.Z = RaycastHeight;
	// To stays the same since where shooting at the same TargetEdge.
	World->LineTraceSingleByChannel(HitResult, From, From + 2  *(To - From), ECollisionChannel::ECC_Camera, Params);
	// Value third raycast.
	if(FVector::DistSquaredXY(To, HitResult.Location) > RaycastDistanceThreshold)
		return false;

	// There is a clear path from the Shooter to the Target via all edges.
	if(bDebugDrawPaths) DrawLine(FVector2D(From), FVector2D(HitResult.Location), FColor::Orange);
	if(bDebugDrawPaths) DrawLine(HitBackup1.Start, HitBackup1.End, FColor::Orange);
	if(bDebugDrawPaths) DrawLine(HitBackup2.Start, HitBackup2.End, FColor::Orange);
	return true;
}


// Fill out your copyright notice in the Description page of Project Settings.

#ifndef COLLISION_BULLET_TRACE
#define COLLISION_BULLET_TRACE ECC_GameTraceChannel3
#endif

#include "PanzerspielGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "TankPawn.h"
#include "Utility.h"
#include "BulletPath.h"
#include "DrawDebugHelpers.h"


APanzerspielGameModeBase::APanzerspielGameModeBase() {
	if(DebugLog) UE_LOG(LogTemp, Warning, TEXT("Called APanzerspielGameModeBase"));
}

TArray<ATankPawn*> APanzerspielGameModeBase::GetTankPawns() const {
	return PlayerPawns;
}

TArray<ATankPawn*> APanzerspielGameModeBase::GetTankPawnsByTeam(int32 TeamIndex){
	TArray<ATankPawn*> FilteredPawns;
	for(ATankPawn *Pawn: PlayerPawns) {
		if(Pawn->GetTeam() == TeamIndex)
		FilteredPawns.Add(Pawn);
	}
	return FilteredPawns;
}

TArray<ATankPawn*> APanzerspielGameModeBase::GetTankPawnsByTeams(TArray<int32> TeamIndices) {
	TArray<ATankPawn*> FilteredPawns;
	for(ATankPawn *Pawn: PlayerPawns) {
		if(TeamIndices.Contains(Pawn->GetTeam()))
			FilteredPawns.Add(Pawn);
	}
	return FilteredPawns;
}

TArray<ATankPawn*> APanzerspielGameModeBase::GetTankPawnNotInTeam(int32 TeamIndex) {
	TArray<ATankPawn*> FilteredPawns;
	for(ATankPawn *Pawn: PlayerPawns) {
		if(IsValid(Pawn) && Pawn->GetTeam() != TeamIndex)
			FilteredPawns.Add(Pawn);
	}
	return FilteredPawns;
}

void APanzerspielGameModeBase::TankPawnRemoveSelf(ATankPawn* Self) {
	PlayerPawns.Remove(Self);
}

void APanzerspielGameModeBase::TankPawnRegisterSelf(ATankPawn* Self) {
	PlayerPawns.Add(Self);
}

void APanzerspielGameModeBase::ClearTankPawns() {
	PlayerPawns.Empty();
}

// Collects all players possibly visible edges.
void APanzerspielGameModeBase::GetAllPlayerEdges() {
	for(ATankPawn *TankPawn : PlayerPawns) {
		// Only really need this the first time.
		TArray<UObstacleEdge*> Edges;
		const FVector2D TankLocation = FVector2D(TankPawn->GetActorLocation());
		for(const AWorldObstacle *Obstacle : Obstacles)
			Edges.Append(Obstacle->GetPossibleEdges(TankLocation));
		PlayersEdges.Add(TankPawn, Edges);
		// Probably increases performance.
		Edges.Sort();
	}
}

// Gets all Obstacles in the game and adds them to the array Obstacles.
void APanzerspielGameModeBase::PopulateObstacles() {
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, AWorldObstacle::StaticClass(), FoundActors);
	for(AActor *Actor : FoundActors)
		Obstacles.Add(Cast<AWorldObstacle>(Actor));
}

TArray<UObstacleEdge*>& APanzerspielGameModeBase::GetPlayersEdges(const AActor* TankPawn) {
	// We might already calculated the edges that are visible from this tankPawn.
	if(!PlayersEdges.Contains(TankPawn)) {
		// The edges visible from this tank have not been calculated yet.
		TArray<UObstacleEdge*> Edges;
		const FVector2D TankLocation = FVector2D(TankPawn->GetActorLocation());
		for(const AWorldObstacle *Obstacle : Obstacles)
			Edges.Append(Obstacle->GetPossibleEdges(TankLocation));
		// Probably increases performance.
		Edges.Sort();
		PlayersEdges.Add(TankPawn, Edges);
	}

	return PlayersEdges.Find(TankPawn)->Edges;
}

void APanzerspielGameModeBase::AddWorldObstacle(const AWorldObstacle* Obstacle) {
	Obstacles.Add(Obstacle);
}

void APanzerspielGameModeBase::RemoveWorldObstacle(const AWorldObstacle* Obstacle) {
	Obstacles.Remove(Obstacle);
}

bool APanzerspielGameModeBase::FindDirectPath(FBulletPath& BulletPath, const AActor* Origin, const FVector& OriginLocation, const AActor* Target) {
	UWorld *World = Origin->GetWorld();
	if(!World)
		return false;
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Origin);
	World->LineTraceSingleByChannel(HitResult, OriginLocation, Target->GetActorLocation(), COLLISION_BULLET_TRACE, Params);
	// Middle trace did not reach the target edge.
	if(HitResult.Actor != Target)
		return false;
	// Do right trace.
	float HitDistance = HitResult.Distance;
	const FVector2D OriginLoc2D = FVector2D(OriginLocation);
	const FVector2D ShootingDirection = FVector2D(Target->GetActorLocation() - OriginLocation).GetSafeNormal();
	const FVector2D OrthogonalNormal = FVector2D(ShootingDirection.Y, -ShootingDirection.X); // Already normalized.
	const FVector2D RightStartLoc = OriginLoc2D + OrthogonalNormal * BulletRadius;
	const FVector2D RightEndLoc = RightStartLoc + 32 * HitDistance * ShootingDirection; // I chose 32 just to make sure its long enough.
	World->LineTraceSingleByChannel(HitResult, FVector(RightStartLoc.X, RightStartLoc.Y, RaycastHeight), FVector(RightEndLoc.X, RightEndLoc.Y, RaycastHeight), COLLISION_BULLET_TRACE, Params);
	
	const bool RightIsLonger = HitResult.Distance > HitDistance;
	const bool RightHitTarget = HitResult.Actor == Target;
	// One of both has to be true, otherwise the bullet will definitely not reach the target.
	if(!RightIsLonger && !RightHitTarget)
		return false;

	// Do left trace.
	const FVector2D LeftStartLoc = OriginLoc2D - OrthogonalNormal * BulletRadius;
	const FVector2D LeftEndLoc = LeftEndLoc + 32 * HitDistance * ShootingDirection;
	World->LineTraceSingleByChannel(HitResult, FVector(LeftStartLoc.X, LeftStartLoc.Y, RaycastHeight), FVector(LeftEndLoc.X, LeftEndLoc.Y, RaycastHeight), COLLISION_BULLET_TRACE, Params);

	const bool LeftIsLonger = HitResult.Distance > HitDistance;
	const bool LeftHitTarget = HitResult.Actor == Target;
	// Again one of both must be true.
	if(!LeftIsLonger && !LeftHitTarget)
		return false;

	// Everything was successful.
	BulletPath.Target = Target->GetActorLocation();
	BulletPath.PathLength = HitResult.Distance;
	return true;
}

bool APanzerspielGameModeBase::FindSingleRicochetPath(TArray<FBulletPath> &BulletPaths, const AActor *Origin, const FVector& OriginLocation, const TArray<UObstacleEdge*> &OriginEdges,
	const AActor *Target, const TArray<UObstacleEdge*> &TargetEdges) {
	if(!(IsValid(Origin) && IsValid(Target)))
		return false;
	
	// TODO: Doing the gathering and intersecting of the edges both in the same loop could improve the performance, the downside is that we then cant use the potentially visible edges for comparison with other tanks.
	// Only keep edges that are visible from both locations.
	TArray<UObstacleEdge*> IntersectedEdges = UUtility::IntersectArrays(OriginEdges, TargetEdges);

	// Only keep edges that can reflect the bullet to the target according to their rotation.
	const FVector2D TargetLocation = FVector2D(Target->GetActorLocation());
	TArray<UObstacleEdge*> FilteredEdges;
	for(UObstacleEdge* Edge : IntersectedEdges)
		if(UUtility::CanBulletEverHitTarget(Edge, FVector2D(OriginLocation), TargetLocation, GetWorld()))
			FilteredEdges.Add(Edge);
	// Make sure its empty.
	IntersectedEdges.Empty();

	for(const UObstacleEdge* Edge : FilteredEdges)
		UUtility::FilterSingleRicochetLOS(Edge, Origin, OriginLocation, Target, RaycastHeight, BulletPaths);

	//if(bDebugDrawRaycastCalculation) ShowBulletPaths(BulletPaths);

	if(BulletPaths.Num() <= 0)
		return false;
	else
		return true;
}

bool APanzerspielGameModeBase::FindDoubleRicochetPath(const AActor *Origin, const FVector& OriginLocation, const TArray<UObstacleEdge*> &OriginEdges,
	const AActor *Target, const TArray<UObstacleEdge*> &TargetEdges, TArray<FBulletPath> &BulletPaths) {
	// Debug
	const double Start = FPlatformTime::Seconds();
	int32 FoundCounter = 0;
	
	if(!(Origin && Target))
		return false;
	// Get some variables that we'll need.
	const FVector2D ShooterLocation = FVector2D(OriginLocation);
	const FVector2D TargetLocation = FVector2D(Target->GetActorLocation());

	// Get all edges that are visible from the shooter and all that are visible from the target.
	for(const UObstacleEdge* TargetEdge : TargetEdges) {
		// Check whether the other edge can be seen by this one (kinda implemented in a later step).
		const FVector2D MirroredTarget = UUtility::MirrorPoint(TargetLocation, TargetEdge->Start, TargetEdge->End - TargetEdge->Start);
		for(const UObstacleEdge* ShooterEdge : OriginEdges) {
			const FVector2D TwiceMirroredTarget = UUtility::MirrorPoint(MirroredTarget, ShooterEdge->Start, ShooterEdge->End - ShooterEdge->Start);
			const FVector2D ShootDirection = TwiceMirroredTarget - ShooterLocation;
			// Interestingly this test is not obsolete by to the following test. This is the case because it does not
			// detect when the shooting direction points in the exact opposite direction.
			const FVector2D ShooterEdgeMiddle = ShooterEdge->Start + (ShooterEdge->End - ShooterEdge->Start)/2;
			if((ShootDirection | (ShooterEdgeMiddle - ShooterLocation)) < 0)
				continue;
			const FVector2D ShootersEdgeStart = ShooterEdge->Start - ShooterLocation;
			const FVector2D ShootersEdgeEnd = ShooterEdge->End - ShooterLocation;
			// Check whether ShootDirection lies between ShootersEdgeStart and ShootersEdgeEnd.
			if((ShootDirection ^ ShootersEdgeStart) * (ShootDirection ^ ShootersEdgeEnd) >= 0)
				continue;
			const FVector2D TargetEdgeNormal = FVector2D(TargetEdge->End.Y - TargetEdge->Start.Y, -(TargetEdge->End.X - TargetEdge->Start.X));
			const FVector2D ShooterEdgeNormal = FVector2D(ShooterEdge->End.Y - ShooterEdge->Start.Y, -(ShooterEdge->End.X - ShooterEdge->Start.X));
			const FVector2D MirroredShootDirection = UUtility::MirrorVector(ShootDirection, ShooterEdge->Start, ShooterEdgeNormal);
			if((MirroredShootDirection | TargetEdgeNormal) <= 0)
				continue;
			// Check whether the TargetEdge and the ShooterEdge are not facing away from each other and thus can reflect a bullet properly.
			if(UUtility::AreFacingAway(TargetEdge, ShooterEdge, TargetEdgeNormal))
				continue;
			if(!UUtility::IsReflectionGonnaHit(ShooterEdge, TargetEdge, ShooterEdgeNormal, ShooterLocation, ShootDirection))
				continue;
			// If we reach this point this is a possible edge combination.
			FBulletPath BulletPath;
			if(!UUtility::HasDoubleRicochetLOS(ShooterEdge, TargetEdge, Origin, OriginLocation, Target, ShootDirection.GetSafeNormal(), RaycastHeight, BulletPath))
				continue;
			// If we reach this point this is most likely a valid edge combination.
			++FoundCounter;
			BulletPaths.Add(BulletPath);
		}
	}

	const double End = FPlatformTime::Seconds();
	if(bDebugLog) UE_LOG(LogTemp, Warning, TEXT("code executed in %f seconds, found %d edges."), End-Start, FoundCounter);

	if(BulletPaths.Num() <= 0)
		return false;

	return true;
}

bool APanzerspielGameModeBase::GetDirectPath(const AActor* Origin, const FVector &OriginLocation, const AActor* Target, FVector &OutTargetLocation) {
	FBulletPath BulletPath;
	FindDirectPath(BulletPath, Origin, OriginLocation, Target);
	if(BulletPath.PathLength <= 1)
		return false;
	OutTargetLocation = BulletPath.Target;
	return true;
}

bool APanzerspielGameModeBase::GetShortestSingleRicochet(const AActor* Origin, const FVector &OriginLocation, const AActor* Target, FVector &OutTargetLocation) {
	TArray<FBulletPath> BulletPaths;
	TArray<UObstacleEdge*> &OriginEdges = GetPlayersEdges(Origin);
	TArray<UObstacleEdge*> &TargetEdges = GetPlayersEdges(Target);
	FindSingleRicochetPath(BulletPaths, Origin, OriginLocation, OriginEdges, Target, TargetEdges);

	if(BulletPaths.Num() <= 0)
		return false;
	
	float ShortestPath = std::numeric_limits<float>::max();
	for(FBulletPath &Path : BulletPaths) {
		if(Path.PathLength < ShortestPath) {
			ShortestPath = Path.PathLength;
			OutTargetLocation = Path.Target;
		}
	}

	return true;
}

bool APanzerspielGameModeBase::GetShortestDoubleRicochet(const AActor* Origin, const FVector &OriginLocation, const AActor* Target, FVector &OutTargetLocation) {
	TArray<FBulletPath> BulletPaths;
	TArray<UObstacleEdge*> &OriginEdges = GetPlayersEdges(Origin);
	TArray<UObstacleEdge*> &TargetEdges = GetPlayersEdges(Target);
	FindDoubleRicochetPath(Origin, OriginLocation, OriginEdges, Target, TargetEdges, BulletPaths);
	
	if(BulletPaths.Num() <= 0)
		return false;
	
	float ShortestPath = std::numeric_limits<float>::max();
	for(FBulletPath &Path : BulletPaths) {
		if(Path.PathLength < ShortestPath) {
			ShortestPath = Path.PathLength;
			OutTargetLocation = Path.Target;
		}
	}
	return true;
}

void APanzerspielGameModeBase::LevelStreamingBegan() {
	InvalidateEdges();
	Obstacles.Empty();
}

void APanzerspielGameModeBase::LevelStreamingEnded() {
	InvalidateEdges();
	PopulateObstacles();
}

void APanzerspielGameModeBase::BeginPlay() {
	Super::BeginPlay();
	
	// TODO: Theres a third crosshair in the game when we have two players.
	/*if(PlayerControllers.Num() < MaxPlayers) {
		// Create New Player.
		APlayerController *Controller = UGameplayStatics::CreatePlayer(this);
		PlayerControllers.Init(Controller, 0);
	}*/
	PopulateObstacles();
}

void APanzerspielGameModeBase::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	InvalidateEdges();
}

void APanzerspielGameModeBase::InvalidateEdges() {
	PlayersEdges.Empty();
}

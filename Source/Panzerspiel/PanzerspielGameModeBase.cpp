// Fill out your copyright notice in the Description page of Project Settings.


#include "PanzerspielGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "TankPawn.h"
#include "Utility.h"
#include "BulletPath.h"


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
		TArray<FObstacleEdge> Edges;
		FVector2D TankLocation = FVector2D(TankPawn->GetActorLocation());
		for(const AWorldObstacle *Obstacle : Obstacles)
			Edges.Append(Obstacle->GetPossibleEdges2(TankLocation));
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

bool APanzerspielGameModeBase::FindDirectPath(FBulletPath& BulletPath, const AActor* Origin, const AActor* Target) {
	UWorld *World = Origin->GetWorld();
	if(!World)
		return false;
	
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Origin);
	World->LineTraceSingleByChannel(HitResult, Origin->GetActorLocation(), Target->GetActorLocation(), ECC_Camera, Params);

	if(HitResult.Actor == Target) {
		BulletPath.Target = Target->GetActorLocation();
		BulletPath.PathLength = HitResult.Distance;
		return true;
	}

	return false;
}

bool APanzerspielGameModeBase::FindSingleRicochetPath(TArray<FBulletPath> &BulletPaths, const AActor *Origin, const TArray<FObstacleEdge> &OriginEdges,
	const AActor *Target, const TArray<FObstacleEdge> &TargetEdges) {
	
	const double Start = FPlatformTime::Seconds();

	if(!(IsValid(Origin) && IsValid(Target)))
		return false;
	// TODO: Doing the gathering and intersecting of the edges both in the same loop could improve the performance, the downside is that we then cant use the potentially visible edges for comparison with other tanks.
	// Only keep edges that are visible from both locations.
	TArray<FObstacleEdge> IntersectedEdges = FUtility::IntersectArrays(OriginEdges, TargetEdges);

	// Only keep edges that can reflect the bullet to the target according to their rotation.
	const FVector2D OriginLocation = FVector2D(Origin->GetActorLocation());
	const FVector2D TargetLocation = FVector2D(Target->GetActorLocation());
	TArray<FObstacleEdge> FilteredEdges;
	for(const FObstacleEdge &Edge : IntersectedEdges)
		if(FUtility::CanBulletEverHitTarget(Edge, OriginLocation, TargetLocation))
			FilteredEdges.Add(Edge);
	// Make sure its empty.
	IntersectedEdges.Empty();

	for(const FObstacleEdge &Edge : FilteredEdges)
		FUtility::FilterSingleRicochetLOS(Edge, Origin, Target, RaycastHeight, HitThreshold, BulletPaths);

	//if(bDebugDrawRaycastCalculation) ShowBulletPaths(BulletPaths);

	const double End = FPlatformTime::Seconds();
	if (bDebugLog) UE_LOG(LogTemp, Warning, TEXT("code executed in %f seconds, found %d edges."), End-Start, BulletPaths.Num());

	if(BulletPaths.Num() <= 0)
		return false;
	else
		return true;
}

bool APanzerspielGameModeBase::FindDoubleRicochetPath(const AActor *Origin, const TArray<FObstacleEdge> &OriginEdges,
	const AActor *Target, const TArray<FObstacleEdge> &TargetEdges, TArray<FBulletPath> &BulletPaths) {
	// Debug
	const double Start = FPlatformTime::Seconds();
	int32 FoundCounter = 0;
	
	if(!(Origin && Target))
		return false;
	// Get some variables that we'll need.
	const FVector2D ShooterLocation = FVector2D(Origin->GetActorLocation());
	const FVector2D TargetLocation = FVector2D(Target->GetActorLocation());

	// Get all edges that are visible from the shooter and all that are visible from the target.
	for(const FObstacleEdge &TargetEdge : TargetEdges) {
		// Check whether the other edge can be seen by this one (kinda implemented in a later step).
		const FVector2D MirroredTarget = FUtility::MirrorPoint(TargetLocation, TargetEdge.Start, TargetEdge.End - TargetEdge.Start);
		for(const FObstacleEdge &ShooterEdge : OriginEdges) {
			const FVector2D TwiceMirroredTarget = FUtility::MirrorPoint(MirroredTarget, ShooterEdge.Start, ShooterEdge.End - ShooterEdge.Start);
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
			const FVector2D MirroredShootDirection = FUtility::MirrorVector(ShootDirection, ShooterEdge.Start, ShooterEdgeNormal);
			if((MirroredShootDirection | TargetEdgeNormal) <= 0)
				continue;
			// Check whether the TargetEdge and the ShooterEdge are not facing away from each other and thus can reflect a bullet properly.
			if(FUtility::AreFacingAway(TargetEdge, ShooterEdge, TargetEdgeNormal))
				continue;
			if(!FUtility::IsReflectionGonnaHit(ShooterEdge, TargetEdge, ShooterEdgeNormal, ShooterLocation, ShootDirection))
				continue;
			// If we reach this point this is a possible edge combination.
			FBulletPath BulletPath;
			if(!FUtility::HasDoubleRicochetLOS(ShooterEdge, TargetEdge, Origin, Target, ShootDirection, RaycastHeight, DistanceThreshold, BulletPath))
				continue;
			// If we reach this point this is most likely a valid edge combination.
			++FoundCounter;
			BulletPaths.Add(BulletPath);
			/*if(FoundCounter >= FirstEdgeToShow && FoundCounter <= LastEdgeToShow) {
				if(bDebugDrawCombinations) DrawEdge(TargetEdge, FColor::Red);
				if(bDebugDrawCombinations) DrawEdge(ShooterEdge, FColor::Green);
			}*/
		}
	}

	const double End = FPlatformTime::Seconds();
	if(bDebugLog) UE_LOG(LogTemp, Warning, TEXT("code executed in %f seconds, found %d edges."), End-Start, FoundCounter);

	if(BulletPaths.Num() <= 0)
		return false;

	return true;
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
	GetAllPlayerEdges();
}

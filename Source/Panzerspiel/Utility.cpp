// All rights reserved @Apfelstrudel Games.


#include "Utility.h"
#include "BulletPath.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

UBroadLineTraceEdgeParams::UBroadLineTraceEdgeParams() {
}

UBroadLineTraceEdgeParams* UBroadLineTraceEdgeParams::Init(float NRadius, FVector2D NShotDir,
	UObstacleEdge* NTargetEdge, const AActor* NIgnoreActor, FVector2D NOriginLoc, float NTraceDistance,
	float NTraceHeight, float NDistanceThreshold) {

	Radius = NRadius;
	ShotDir = NShotDir;
	TargetEdge = NTargetEdge;
	IgnoreActor = NIgnoreActor;
	OriginLoc = NOriginLoc;
	TraceDistance = NTraceDistance;
	TraceHeight = NTraceHeight;
	DistanceThreshold = NDistanceThreshold;
	return this;
}

UBroadLineTraceTargetParams::UBroadLineTraceTargetParams() {
}

UBroadLineTraceTargetParams* UBroadLineTraceTargetParams::Init(float NRadius, FVector2D NShotDir, const AActor* NTarget,
	const AActor* NIgnoreActor, FVector2D NOriginLoc, float NTraceDistance, float NTraceHeight, float NDistanceThreshold) {

	Radius = NRadius;
	ShotDir = NShotDir;
	Target = NTarget;
	IgnoreActor = NIgnoreActor;
	OriginLoc = NOriginLoc;
	TraceDistance = NTraceDistance;
	TraceHeight = NTraceHeight;
	DistanceThreshold = NDistanceThreshold;
	return this;
}

TArray<UObstacleEdge*> UUtility::IntersectArrays(const TArray<UObstacleEdge*> &First, const TArray<UObstacleEdge*> &Second) {
	TArray<UObstacleEdge*> Intersection;
	const int32 LengthFirst = First.Num();
	const int32 LengthSecond = Second.Num();
	int32 i = 0;
	int32 j = 0;
	while (i < LengthFirst && j < LengthSecond) {
		if (First[i] < Second[j])
			i++;
		else if (Second[j] < First[i])
			j++;
		else if (First[i] == Second[j]) {
			Intersection.Add(Second[j]);
			i++;
			j++;
		}
	}
	return Intersection;
}

FVector2D UUtility::MirrorVector(const FVector2D ToMirror, const FVector2D MirrorOrigin, const FVector2D MirrorDirection) {
	// TODO: Maybe it would be faster to change this to use the same method as FVector::MirrorByVector which uses one square root but simplifies the rest of the calculation dramatically.
	// U = MirrorOrigin; d = ToMirror; n = MirrorDirection; D = D;
	const FVector2D PointToMirror = MirrorOrigin + ToMirror;
	// 1. Step.
	const FVector2D Tmp = MirrorOrigin - PointToMirror;
	// 2. Step.
	const float CleanScalar = FVector2D::DotProduct(Tmp, MirrorDirection);
	const float VariableScalar = FVector2D::DotProduct(MirrorDirection, MirrorDirection);
	// In Noteability: t.
	const float Theta = -CleanScalar / VariableScalar;
	// 3. Step.
	const FVector2D Intersection = MirrorOrigin + Theta * MirrorDirection;
	// 4. Step.
	const FVector2D PointToIntersection = Intersection - PointToMirror;
	// 5. Step.
	const FVector2D MirroredPoint = PointToMirror + 2 * PointToIntersection;
	// 6. Step.
	const FVector2D MirroredVector = MirroredPoint - MirrorOrigin;
	
	return MirroredVector;
}

FVector2D UUtility::MirrorPoint(const FVector2D ToMirror, const FVector2D MirrorOrigin, const FVector2D MirrorDirection) {
	// U = MirrorOrigin; d = ToMirror; n = MirrorDirection;
	// 1. Step.
	const FVector2D Tmp = MirrorOrigin - ToMirror;
	// 2. Step.
	const float CleanScalar = FVector2D::DotProduct(Tmp, MirrorDirection);
	const float VariableScalar = FVector2D::DotProduct(MirrorDirection, MirrorDirection);
	// In Noteability: t.
	const float Theta = -CleanScalar / VariableScalar;
	// 3. Step.
	const FVector2D Intersection = MirrorOrigin + Theta * MirrorDirection;
	// 4. Step.
	const FVector2D PointToIntersection = Intersection - ToMirror;
	// 5. Step.
	const FVector2D MirroredPoint = ToMirror + 2 * PointToIntersection;

	return MirroredPoint;
}

bool UUtility::CanBulletEverHitTarget(const UObstacleEdge* Edge, const FVector2D &BulletOrigin, const FVector2D &Target) {
	const FVector2D EdgeMiddle = Edge->Start + (Edge->End - Edge->Start) / 2;
	const FVector2D EdgeDirection = Edge->End - Edge->Start;
	const FVector2D EdgeNormal = FVector2D(EdgeDirection.Y, -EdgeDirection.X);
	// Create vector from bullet to edge corner.
	const FVector2D StartDirection = BulletOrigin - Edge->Start;
	const FVector2D EndDirection = BulletOrigin - Edge->End;
	// Mirror that vector.
	const FVector2D MirroredStartDirection = MirrorVector(StartDirection, EdgeMiddle, EdgeNormal);
	const FVector2D MirroredEndDirection = MirrorVector(EndDirection, EdgeMiddle, EdgeNormal);
	// Create vector from middle of edge to target.
	const FVector2D StartTargetDirection = Target - Edge->Start;
	const FVector2D EndTargetDirection = Target - Edge->End;
	
	/*if(bDebugDrawEdgeCalculation) {
		// StartDirection and MirroredStartDirection in blue.
		DrawDebugLine(GetWorld(), FVector(Edge.Start.X, Edge.Start.Y, DisplayHeight),
			FVector(Edge.Start.X + StartDirection.X, Edge.Start.Y + StartDirection.Y, DisplayHeight),
			FColor::Blue, false, -1, 0, LineThickness);
		DrawDebugLine(GetWorld(), FVector(Edge.Start.X, Edge.Start.Y, DisplayHeight),
            FVector(Edge.Start.X + MirroredStartDirection.X, Edge.Start.Y + MirroredStartDirection.Y,
            	DisplayHeight), FColor::Blue, false, -1, 0, LineThickness);

		// EndDirection and MirroredEndDirection in purple.
		DrawDebugLine(GetWorld(), FVector(Edge.End.X, Edge.End.Y, DisplayHeight),
            FVector(Edge.End.X + EndDirection.X, Edge.End.Y + EndDirection.Y, DisplayHeight),
            FColor::Purple, false, -1, 0, LineThickness);
		DrawDebugLine(GetWorld(), FVector(Edge.End.X, Edge.End.Y, DisplayHeight),
            FVector(Edge.End.X + MirroredEndDirection.X, Edge.End.Y + MirroredEndDirection.Y,
                DisplayHeight), FColor::Purple, false, -1, 0, LineThickness);

		// StartTargetDirection and EndTargetDirection in Green.
		DrawDebugLine(GetWorld(), FVector(Edge.Start.X, Edge.Start.Y, DisplayHeight), FVector(Target.X,
			Target.Y, DisplayHeight), FColor::Green, false, -1, 0, LineThickness);// TargetDirection in Green.
		DrawDebugLine(GetWorld(), FVector(Edge.End.X, Edge.End.Y, DisplayHeight), FVector(Target.X,
			Target.Y, DisplayHeight), FColor::Green, false, -1, 0, LineThickness);
	}*/
	
	// If the sign from the cross products of both the left with the middle and the right and the middle have a different
	// sign then the target could possibly get hit.
	return (StartTargetDirection ^ MirroredStartDirection) * (EndTargetDirection ^ MirroredEndDirection) < 0;
}

void UUtility::FilterSingleRicochetLOS(const UObstacleEdge* Edge, const AActor *Origin, const FVector& OriginLocation,
	const AActor *Target, float RaycastHeight, float HitThreshold, TArray<FBulletPath> &BulletPaths){

	UWorld *World = Target->GetWorld();
	if(!World)
		return;
	// Mirror target at the edge.
	const FVector2D TargetLocation = FVector2D(Target->GetActorLocation());
	FVector2D MirroredTarget = MirrorPoint(TargetLocation, Edge->Start, Edge->End - Edge->Start);
	// Do raycast from the origin and check if it hit the edge.
	FHitResult HitResult;
	FCollisionQueryParams Params;
	// TODO: TankPawn should not be hardcoded right here as well as below.
	Params.AddIgnoredActor(Origin);
	World->LineTraceSingleByChannel(HitResult, FVector(OriginLocation.X, OriginLocation.Y, RaycastHeight),
		FVector(MirroredTarget.X, MirroredTarget.Y, RaycastHeight), ECollisionChannel::ECC_Camera, Params);
	const FVector2D EdgeDirection = Edge->End - Edge->Start;
	FVector2D HitLocation = FVector2D(HitResult.Location.X, HitResult.Location.Y);
	float CrossProduct = FVector2D::CrossProduct(EdgeDirection, (HitLocation - Edge->Start));
	if(FMath::Abs(CrossProduct) > HitThreshold) {
		// We hit something else than the edge.
		return;
	}
	// Store the full PathLength.
	float PathLength = HitResult.Distance;

	// Do raycast from the target and check if it hit the edge.
	Params.ClearIgnoredActors();
	Params.AddIgnoredActor(Target);
	// We do the raycast to the last hit location and multiply it by two just to make sure it does not stop directly in front of it.
	FVector RaycastOrigin = FVector(TargetLocation.X, TargetLocation.Y, RaycastHeight);
	FVector RaycastTarget = HitResult.Location + HitResult.Location - RaycastOrigin;
	World->LineTraceSingleByChannel(HitResult, FVector(TargetLocation.X, TargetLocation.Y, RaycastHeight),
        RaycastTarget, ECollisionChannel::ECC_Camera, Params);
	HitLocation = FVector2D(HitResult.Location.X, HitResult.Location.Y);
	CrossProduct = FVector2D::CrossProduct(EdgeDirection, (HitLocation - Edge->Start));
	if(FMath::Abs(CrossProduct) > HitThreshold) {
		// We hit something other than the edge.
		return; 
	}

	// We hit both edges.
	PathLength += HitResult.Distance;
	// TODO: If weird bug appear this might be the reason.
	FBulletPath Path = FBulletPath(FVector(MirroredTarget.X, MirroredTarget.Y, 0) , PathLength);
	BulletPaths.Add(Path);
}

bool UUtility::AreFacingAway(const UObstacleEdge* Edge1, const UObstacleEdge* Edge2, const FVector2D& Edge1Normal) {
	const FVector2D Edge1Middle = Edge1->Start + (Edge1->End - Edge1->Start)/2;
	const FVector2D Edge2Middle = Edge2->Start + (Edge2->End - Edge2->Start)/2;
	const FVector2D Edge2ToEdge1Direction = Edge1Middle - Edge2Middle;
	return (Edge1Normal | Edge2ToEdge1Direction) >= 0;
}

bool UUtility::IsReflectionGonnaHit(const UObstacleEdge* ShooterEdge, const UObstacleEdge* TargetEdge,
	const FVector2D& ShooterEdgeNormal, const FVector2D& ShooterLocation, const FVector2D& ShootDirection) {
	const FVector2D ShooterEdgeIntersect = CalculateIntersect(ShooterEdge->Start, ShooterEdge->End - ShooterEdge->Start, ShooterLocation, ShootDirection);
	const FVector2D ShooterMirroredShootDirection = MirrorPoint(ShooterLocation, ShooterEdgeIntersect, ShooterEdgeNormal) - ShooterEdgeIntersect;
	const FVector2D StartDirection = TargetEdge->Start - ShooterEdgeIntersect;
	const FVector2D EndDirection = TargetEdge->End - ShooterEdgeIntersect;
	return (ShooterMirroredShootDirection ^ StartDirection) * (ShooterMirroredShootDirection ^ EndDirection) < 0;
}

FVector2D UUtility::CalculateIntersect(const FVector2D& Edge1Start, const FVector2D& Edge1Dir,
	const FVector2D& Edge2Start, const FVector2D& Edge2Dir) {
	
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

bool UUtility::HasDoubleRicochetLOS(const UObstacleEdge* ShooterEdge, const UObstacleEdge* TargetEdge, const AActor* Shooter, const FVector& ShooterLocation,
	const AActor* Target, const FVector2D& ShootDirection, const float RaycastHeight, const float DistanceThreshold, FBulletPath &BulletPath) {
	UWorld *World = Shooter->GetWorld();
	if(!World) return false;
	
	// Setup first raycast (Shooter -> ShooterEdge).
	FVector From = ShooterLocation;
	// TODO: These calculations are already done in IsReflectionGonnaHit, it might be better to safe them somewhere for later use.
	const FVector2D ShooterEdgeIntersect = CalculateIntersect(ShooterEdge->Start, ShooterEdge->End - ShooterEdge->Start, FVector2D(From), ShootDirection);
	FHitResult HitResult;
	FVector To = FVector(ShooterEdgeIntersect.X, ShooterEdgeIntersect.Y, RaycastHeight);
	if(!HasLOSToTarget(From, To, Shooter, HitResult, RaycastHeight, DistanceThreshold))
		return false;
	// Do the other two traces.
	// Do right trace.
	const float BulletRadius = 15;
	float HitDistance = HitResult.Distance;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Shooter);
	const FVector2D OriginLoc2D = FVector2D(From);
	const FVector2D OrthogonalNormal = FVector2D(ShootDirection.Y, -ShootDirection.X); // Already normalized.
	const FVector2D RightStartLoc = OriginLoc2D + OrthogonalNormal * BulletRadius;
	const FVector2D RightEndLoc = RightStartLoc + 32 * HitDistance * ShootDirection; // I chose 32 just to make sure its long enough.
	World->LineTraceSingleByChannel(HitResult, FVector(RightStartLoc.X, RightStartLoc.Y, RaycastHeight), FVector(RightEndLoc.X, RightEndLoc.Y, RaycastHeight), ECC_Camera, Params);
	// Check if the right HitLocations distance to the line is smaller than the threshold.
	if(!IsPointOnLine(FVector2D(HitResult.Location), ShooterEdge, DistanceThreshold))
		return false;
	// Do left trace.
	const FVector2D LeftStartLoc = OriginLoc2D - OrthogonalNormal * BulletRadius;
	const FVector2D LeftEndLoc = LeftEndLoc + 32 * HitDistance * ShootDirection;
	World->LineTraceSingleByChannel(HitResult, FVector(LeftStartLoc.X, LeftStartLoc.Y, RaycastHeight), FVector(LeftEndLoc.X, LeftEndLoc.Y, RaycastHeight), ECC_Camera, Params);
	// Check if the right HitLocations distance to the line is smaller than the threshold.
	if(!IsPointOnLine(FVector2D(HitResult.Location), ShooterEdge, DistanceThreshold))
		return false;
	const UObstacleEdge* HitBackup1 = NewObject<UObstacleEdge>()->Init(FVector2D(From), FVector2D(To));

	// Make a copy since we're changing it below.
	BulletPath.Target = FVector(To);
	BulletPath.PathLength = HitResult.Distance;
	// Setup second raycast (ShooterEdge -> TargetEdge).
	From = FVector(ShooterEdgeIntersect.X, ShooterEdgeIntersect.Y, RaycastHeight);
	const FVector2D ShooterEdgeNormal = FVector2D(ShooterEdge->End.Y - ShooterEdge->Start.Y, -(ShooterEdge->End.X - ShooterEdge->Start.X));
	const FVector2D MirroredShootDirection = MirrorVector(ShootDirection, ShooterEdgeIntersect, ShooterEdgeNormal);
	const FVector2D TargetEdgeIntersect = CalculateIntersect(TargetEdge->Start, TargetEdge->End - TargetEdge->Start, ShooterEdgeIntersect, MirroredShootDirection);
	To = FVector(TargetEdgeIntersect.X, TargetEdgeIntersect.Y, RaycastHeight);
	if(!HasLOSToTarget(From, To, ShooterEdge->Parent, HitResult, RaycastHeight, DistanceThreshold))
		return false;
	const UObstacleEdge* HitBackup2 = NewObject<UObstacleEdge>()->Init(FVector2D(From), FVector2D(To));
	
	BulletPath.PathLength += HitResult.Distance;
	// Setup third raycast (Target -> TargetEdge).
	From = Target->GetActorLocation();
	From.Z = RaycastHeight;
	// To stays the same since where shooting at the same TargetEdge.
	if(!HasLOSToTarget(From, To, Target, HitResult, RaycastHeight, DistanceThreshold))
		return false;

	// Now need to check that theres actually enough space for the bullet.
	DrawLine(FVector2D(From), FVector2D(HitResult.Location), FColor::Orange, Shooter);
	DrawLine(HitBackup1->Start, HitBackup1->End, FColor::Orange, Shooter);
	DrawLine(HitBackup2->Start, HitBackup2->End, FColor::Orange, Shooter);
	BulletPath.PathLength += HitResult.Distance;
	return true;
}

void UUtility::ShowBulletPaths(const TArray<FBulletPath>& BulletPaths, const AActor *Origin, const float LineThickness) {
	UWorld *World = Origin->GetWorld();
	if(!World)
		return;
	
	for(FBulletPath Path : BulletPaths) {
		FVector From = Origin->GetActorLocation();
		FVector To = From + (Path.Target - From) / 2;
		DrawDebugLine(World, From, To, FColor::Green, false, -1, 0, LineThickness);
	}
}

void UUtility::ShowEdges(const TArray<const UObstacleEdge*> &EdgesToShow, const AActor *WorldReference, const float DisplayHeight, const float LineThickness) {
	for (const UObstacleEdge* Edge : EdgesToShow)
		DrawEdge(Edge, FColor::Green, WorldReference, LineThickness, DisplayHeight);
}

void UUtility::DrawEdge(const UObstacleEdge* Edge, const FColor Color, const AActor *WorldReference, const float LineThickness, const float DisplayHeight) {
	DrawLine(Edge->Start, Edge->End, Color, WorldReference, LineThickness, DisplayHeight);
}

void UUtility::DrawLine(const FVector2D &Start, const FVector2D &End, const FColor Color, const AActor *WorldReference, const float LineThickness, const float DisplayHeight) {
	UWorld *World = WorldReference->GetWorld();
	if(!World)
		return;
	
	DrawDebugLine(World, FVector(Start.X, Start.Y, DisplayHeight), FVector(End.X, End.Y, DisplayHeight), Color, false, -1, 0, LineThickness);
}

bool UUtility::HasLOSToTarget(FVector& From, const FVector& Target, const AActor* IgnoreActor, FHitResult& HitResult,
	float RaycastHeight, float DistanceThreshold) {
	UWorld *World = IgnoreActor->GetWorld();
	if(!IsValid(World))
		return false;
	From.Z = RaycastHeight;
	const FVector To = FVector(Target.X, Target.Y, RaycastHeight);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(IgnoreActor);
	// TODO: I guess another collision channel could be less expensive.
	// We do the trace further than we might need to make sure we'll definitely hit the edge.
	World->LineTraceSingleByChannel(HitResult, From, From + 2 * (To - From), ECC_Camera, Params);
	// Value the first raycast, we return false if the ray hit to far away from the supposed hit but we give it a small
	// threshold since raycasts are not that precise and we're using floats.
	return FVector::DistSquaredXY(To, HitResult.Location) <= DistanceThreshold;
}

bool UUtility::IsPointOnLine(const FVector2D& Point, const UObstacleEdge* Edge, const float DistanceThreshold) {
	const FVector2D PointFromOrigin = Point - Edge->Start;
	const FVector2D LineDirection = (Edge->End - Edge->Start).GetSafeNormal();
	const FVector2D ProjectedPoint = LineDirection * (PointFromOrigin | LineDirection);
	return FVector2D::DistSquared(ProjectedPoint, PointFromOrigin) <= DistanceThreshold;
}

float UUtility::BroadLineTraceEdge(UBroadLineTraceEdgeParams* P) {
	UWorld *World = P->IgnoreActor->GetWorld();
	if(!World)
		return -1.f;

	// First trace.
	FHitResult Result;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(P->IgnoreActor);
	const FVector TraceEnd = FVector(P->OriginLoc.X + P->TraceDistance * P->ShotDir.X, P->OriginLoc.Y + P->TraceDistance * P->ShotDir.Y, P->TraceHeight);
	const FVector OriginLoc3D = FVector(P->OriginLoc.X, P->OriginLoc.Y, P->TraceHeight);
	World->LineTraceSingleByChannel(Result, OriginLoc3D, TraceEnd, ECC_Camera, QueryParams);
	if(IsPointOnLine(FVector2D(Result.Location), P->TargetEdge, P->DistanceThreshold))
		return -1.f;

	// Right trace.
	const float MiddleHitDistance = Result.Distance;
	FVector2D OrthoShotDir = FVector2D(P->ShotDir.Y, -P->ShotDir.X);
	FVector OffsetOrigin = OriginLoc3D + FVector(OrthoShotDir.X, OrthoShotDir.Y, 0) * P->Radius;
	FVector OffsetEnd = TraceEnd + FVector(OrthoShotDir.X, OrthoShotDir.Y, 0) * P->Radius;
	World->LineTraceSingleByChannel(Result, OffsetOrigin, OffsetEnd, ECC_Camera, QueryParams);
	const bool IsLongerR = Result.Distance > MiddleHitDistance;
	const bool HitTargetR = IsPointOnLine(FVector2D(Result.Location), P->TargetEdge, P->DistanceThreshold);
	if(!IsLongerR && !HitTargetR)
		return -1.f;

	// Left trace.
	OffsetOrigin = OriginLoc3D - FVector(OrthoShotDir.X, OrthoShotDir.Y, 0) * P->Radius;
	OffsetEnd = TraceEnd - FVector(OrthoShotDir.X, OrthoShotDir.Y, 0) * P->Radius;
	World->LineTraceSingleByChannel(Result, OffsetOrigin, OffsetEnd, ECC_Camera, QueryParams);
	const bool IsLongerL = Result.Distance > MiddleHitDistance;
	const bool HitTargetL = IsPointOnLine(FVector2D(Result.Location), P->TargetEdge, P->DistanceThreshold);
	if(!IsLongerL && !HitTargetL)
		return -1.f;

	// Success.
	return MiddleHitDistance;
}

float UUtility::BroadLineTraceTarget(UBroadLineTraceTargetParams* P) {
	UWorld *World = P->IgnoreActor->GetWorld();
	if(!World)
		return -1.f;

	// First trace.
	FHitResult Result;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(P->IgnoreActor);
	const FVector TraceEnd = FVector(P->OriginLoc.X + P->TraceDistance * P->ShotDir.X, P->OriginLoc.Y + P->TraceDistance * P->ShotDir.Y, P->TraceHeight);
	const FVector OriginLoc3D = FVector(P->OriginLoc.X, P->OriginLoc.Y, P->TraceHeight);
	World->LineTraceSingleByChannel(Result, OriginLoc3D, TraceEnd, ECC_Camera, QueryParams);
	if(Result.Actor == P->Target)
		return -1.f;

	// Right trace.
	const float MiddleHitDistance = Result.Distance;
	FVector2D OrthoShotDir = FVector2D(P->ShotDir.Y, -P->ShotDir.X);
	FVector OffsetOrigin = OriginLoc3D + FVector(OrthoShotDir.X, OrthoShotDir.Y, 0) * P->Radius;
	FVector OffsetEnd = TraceEnd + FVector(OrthoShotDir.X, OrthoShotDir.Y, 0) * P->Radius;
	World->LineTraceSingleByChannel(Result, OffsetOrigin, OffsetEnd, ECC_Camera, QueryParams);
	if(!(Result.Distance > MiddleHitDistance) && !(Result.Actor == P->Target))
		return -1.f;

	// Left trace.
	OffsetOrigin = OriginLoc3D - FVector(OrthoShotDir.X, OrthoShotDir.Y, 0) * P->Radius;
	OffsetEnd = TraceEnd - FVector(OrthoShotDir.X, OrthoShotDir.Y, 0) * P->Radius;
	World->LineTraceSingleByChannel(Result, OffsetOrigin, OffsetEnd, ECC_Camera, QueryParams);
	if(!(Result.Distance > MiddleHitDistance) && !(Result.Actor == P->Target))
		return -1.f;

	// Success.
	return MiddleHitDistance;
}

bool UUtility::HasDoubleRicochetLOS2(const UObstacleEdge* ShooterEdge, const UObstacleEdge* TargetEdge, const AActor* Shooter, const FVector& ShooterLoc,
	const AActor* Target, const FVector2D& ShootingDirection, const float TraceHeight, const float DistanceThreshold, FBulletPath &BulletPath) {
	UWorld *World = Shooter->GetWorld();
	if(!World) return false;
	
	// First raycast (ShooterEdge -> Shooter).
	FVector2D ShotDir = ShootingDirection.GetSafeNormal();
	// TODO: These calculations are already done in IsReflectionGonnaHit, it might be better to safe them somewhere for later use.
	FVector2D ShooterEdgeIntersect = CalculateIntersect(ShooterEdge->Start,ShooterEdge->End - ShooterEdge->Start,FVector2D(ShooterLoc), ShotDir);
	const float BulletRadius = 15;
	UBroadLineTraceTargetParams* TargetParams = NewObject<UBroadLineTraceTargetParams>()->Init(BulletRadius, ShotDir, Shooter,
		ShooterEdge->Parent, ShooterEdgeIntersect, 10000, TraceHeight, DistanceThreshold);
	float HitDistance = BroadLineTraceTarget(TargetParams);
	if(HitDistance < 0.f)
		return false;
	BulletPath.PathLength = HitDistance;

	// Second raycast (ShooterEdge -> TargetEdge).
	const FVector2D ShooterEdgeNormal = FVector2D(ShooterEdge->End.Y - ShooterEdge->Start.Y, -(ShooterEdge->End.X - ShooterEdge->Start.X));
	const FVector2D MirroredShotDir = MirrorVector(ShotDir, ShooterEdgeIntersect, ShooterEdgeNormal);
	UObstacleEdge *TargetEdgeCopy = NewObject<UObstacleEdge>()->Init(TargetEdge); // Only needed because UProperties cant be const.
	UBroadLineTraceEdgeParams* EdgeParams = NewObject<UBroadLineTraceEdgeParams>()->Init(BulletRadius, MirroredShotDir,
		TargetEdgeCopy, Shooter, ShooterEdgeIntersect, 10000, TraceHeight, DistanceThreshold);
	HitDistance = BroadLineTraceEdge(EdgeParams);
	if(HitDistance < 0.f)
		return false;
	BulletPath.PathLength += HitDistance;
	
	// Third raycast (TargetEdge -> Target).
	const FVector2D TargetEdgeIntersect = CalculateIntersect(TargetEdge->Start, TargetEdge->End - TargetEdge->Start, ShooterEdgeIntersect, MirroredShotDir);
	TargetParams->Target = Target;
	TargetParams->IgnoreActor = TargetEdge->Parent;
	TargetParams->OriginLoc = TargetEdgeIntersect;
	TargetParams->ShotDir = (FVector2D(Target->GetActorLocation()) - TargetEdgeIntersect).GetSafeNormal();
	HitDistance = BroadLineTraceTarget(TargetParams);
	if(HitDistance < 0.f)
		return false;
	BulletPath.PathLength += HitDistance;
	BulletPath.Target = ShooterLoc + FVector(ShootingDirection.X, ShootingDirection.Y, 0);

	// Now need to check that theres actually enough space for the bullet.
	/*DrawLine(FVector2D(From), FVector2D(HitResult.Location), FColor::Orange, Shooter);
	DrawLine(HitBackup1.Start, HitBackup1.End, FColor::Orange, Shooter);
	DrawLine(HitBackup2.Start, HitBackup2.End, FColor::Orange, Shooter);*/
	return true;
}
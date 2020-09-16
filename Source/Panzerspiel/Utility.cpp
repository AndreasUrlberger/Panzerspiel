// All rights reserved @Apfelstrudel Games.


#include "Utility.h"
#include "BulletPath.h"
#include "DrawDebugHelpers.h"


TArray<FObstacleEdge> UUtility::IntersectArrays(const TArray<FObstacleEdge> &First, const TArray<FObstacleEdge> &Second) {
	TArray<FObstacleEdge> Intersection;
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

bool UUtility::CanBulletEverHitTarget(const FObstacleEdge& Edge, const FVector2D &BulletOrigin, const FVector2D &Target) {
	const FVector2D EdgeMiddle = Edge.Start + (Edge.End - Edge.Start) / 2;
	const FVector2D EdgeDirection = Edge.End - Edge.Start;
	const FVector2D EdgeNormal = FVector2D(EdgeDirection.Y, -EdgeDirection.X);
	// Create vector from bullet to edge corner.
	const FVector2D StartDirection = BulletOrigin - Edge.Start;
	const FVector2D EndDirection = BulletOrigin - Edge.End;
	// Mirror that vector.
	const FVector2D MirroredStartDirection = MirrorVector(StartDirection, EdgeMiddle, EdgeNormal);
	const FVector2D MirroredEndDirection = MirrorVector(EndDirection, EdgeMiddle, EdgeNormal);
	// Create vector from middle of edge to target.
	const FVector2D StartTargetDirection = Target - Edge.Start;
	const FVector2D EndTargetDirection = Target - Edge.End;
	
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

void UUtility::FilterSingleRicochetLOS(const FObstacleEdge& Edge, const AActor *Origin, const FVector& OriginLocation,
	const AActor *Target, float RaycastHeight, float HitThreshold, TArray<FBulletPath> &BulletPaths){

	UWorld *World = Target->GetWorld();
	if(!World)
		return;
	// Mirror target at the edge.
	const FVector2D TargetLocation = FVector2D(Target->GetActorLocation());
	FVector2D MirroredTarget = MirrorPoint(TargetLocation, Edge.Start, Edge.End - Edge.Start);
	// Do raycast from the origin and check if it hit the edge.
	FHitResult HitResult;
	FCollisionQueryParams Params;
	// TODO: TankPawn should not be hardcoded right here as well as below.
	Params.AddIgnoredActor(Origin);
	World->LineTraceSingleByChannel(HitResult, FVector(OriginLocation.X, OriginLocation.Y, RaycastHeight),
		FVector(MirroredTarget.X, MirroredTarget.Y, RaycastHeight), ECollisionChannel::ECC_Camera, Params);
	const FVector2D EdgeDirection = Edge.End - Edge.Start;
	FVector2D HitLocation = FVector2D(HitResult.Location.X, HitResult.Location.Y);
	float CrossProduct = FVector2D::CrossProduct(EdgeDirection, (HitLocation - Edge.Start));
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
	CrossProduct = FVector2D::CrossProduct(EdgeDirection, (HitLocation - Edge.Start));
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

bool UUtility::AreFacingAway(const FObstacleEdge& Edge1, const FObstacleEdge& Edge2, const FVector2D& Edge1Normal) {
	const FVector2D Edge1Middle = Edge1.Start + (Edge1.End - Edge1.Start)/2;
	const FVector2D Edge2Middle = Edge2.Start + (Edge2.End - Edge2.Start)/2;
	const FVector2D Edge2ToEdge1Direction = Edge1Middle - Edge2Middle;
	return (Edge1Normal | Edge2ToEdge1Direction) >= 0;
}

bool UUtility::IsReflectionGonnaHit(const FObstacleEdge& ShooterEdge, const FObstacleEdge& TargetEdge,
	const FVector2D& ShooterEdgeNormal, const FVector2D& ShooterLocation, const FVector2D& ShootDirection) {
	const FVector2D ShooterEdgeIntersect = CalculateIntersect(ShooterEdge.Start, ShooterEdge.End - ShooterEdge.Start, ShooterLocation, ShootDirection);
	const FVector2D ShooterMirroredShootDirection = MirrorPoint(ShooterLocation, ShooterEdgeIntersect, ShooterEdgeNormal) - ShooterEdgeIntersect;
	const FVector2D StartDirection = TargetEdge.Start - ShooterEdgeIntersect;
	const FVector2D EndDirection = TargetEdge.End - ShooterEdgeIntersect;
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

bool UUtility::HasDoubleRicochetLOS(const FObstacleEdge& ShooterEdge, const FObstacleEdge& TargetEdge, const AActor* Shooter, const FVector& ShooterLocation,
	const AActor* Target, const FVector2D& ShootDirection, const float RaycastHeight, const float DistanceThreshold, FBulletPath &BulletPath) {

	UWorld *World = Shooter->GetWorld();
	if(!World)
		return false;
	// Setup first raycast (Shooter -> ShooterEdge).
	FVector From = ShooterLocation;
	// TODO: These calculations are already done in IsReflectionGonnaHit, it might be better to safe them somewhere for later use.
	const FVector2D ShooterEdgeIntersect = CalculateIntersect(ShooterEdge.Start, ShooterEdge.End - ShooterEdge.Start, FVector2D(From), ShootDirection);
	FHitResult HitResult;
	From.Z = RaycastHeight;
	FVector To = FVector(ShooterEdgeIntersect.X, ShooterEdgeIntersect.Y, RaycastHeight);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Shooter);
	// TODO: I guess another collision channel could be less expensive.
	// We do the trace further than we might need to make sure we'll definitely hit the edge.
	World->LineTraceSingleByChannel(HitResult, From, From + 2 * (To - From), ECC_Camera, Params);
	// Value the first raycast, we return false if the ray hit to far away from the supposed hit but we give it a small
	// threshold since raycasts are not that precise and we're using floats.
	if(FVector::DistSquaredXY(To, HitResult.Location) > DistanceThreshold)
		return false;
	//Debug:
	//FObstacleEdge HitBackup1 = FObstacleEdge(FVector2D(From), FVector2D(HitResult.Location));

	// Make a copy since we're changing it below.
	BulletPath.Target = FVector(To);
	BulletPath.PathLength = HitResult.Distance;
	// Setup second raycast (ShooterEdge -> TargetEdge).
	Params.ClearIgnoredActors();
	Params.AddIgnoredActor(ShooterEdge.Parent);
	From = FVector(ShooterEdgeIntersect.X, ShooterEdgeIntersect.Y, RaycastHeight);
	FVector2D ShooterEdgeNormal = FVector2D(ShooterEdge.End.Y - ShooterEdge.Start.Y, -(ShooterEdge.End.X - ShooterEdge.Start.X));
	FVector2D MirroredShootDirection = MirrorVector(ShootDirection, ShooterEdgeIntersect, ShooterEdgeNormal);
	const FVector2D TargetEdgeIntersect = CalculateIntersect(TargetEdge.Start, TargetEdge.End - TargetEdge.Start, ShooterEdgeIntersect, MirroredShootDirection);
	To = FVector(TargetEdgeIntersect.X, TargetEdgeIntersect.Y, RaycastHeight);
	World->LineTraceSingleByChannel(HitResult, From, From + 2 * (To - From), ECC_Camera, Params);
	// Value second raycast.
	if(FVector::DistSquaredXY(To, HitResult.Location) > DistanceThreshold)
		return false;
	// Debug: 
	//FObstacleEdge HitBackup2 = FObstacleEdge(FVector2D(From), FVector2D(HitResult.Location));

	BulletPath.PathLength += HitResult.Distance;
	// Setup third raycast (Target -> TargetEdge).
	Params.ClearIgnoredActors();
	Params.AddIgnoredActor(Target);
	From = Target->GetActorLocation();
	From.Z = RaycastHeight;
	// To stays the same since where shooting at the same TargetEdge.
	World->LineTraceSingleByChannel(HitResult, From, From + 2  *(To - From), ECC_Camera, Params);
	// Value third raycast.
	if(FVector::DistSquaredXY(To, HitResult.Location) > DistanceThreshold)
		return false;

	// There is a clear path from the Shooter to the Target via all edges.
	/*if(bDebugDrawPaths) DrawLine(FVector2D(From), FVector2D(HitResult.Location), FColor::Orange);
	if(bDebugDrawPaths) DrawLine(HitBackup1.Start, HitBackup1.End, FColor::Orange);
	if(bDebugDrawPaths) DrawLine(HitBackup2.Start, HitBackup2.End, FColor::Orange);*/
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

void UUtility::ShowEdges(const TArray<const FObstacleEdge> &EdgesToShow, const AActor *WorldReference, const float DisplayHeight, const float LineThickness) {
	for (const FObstacleEdge &Edge : EdgesToShow)
		DrawEdge(Edge, FColor::Green, WorldReference, LineThickness, DisplayHeight);
}

void UUtility::DrawEdge(const FObstacleEdge &Edge, const FColor Color, const AActor *WorldReference, const float LineThickness, const float DisplayHeight) {
	DrawLine(Edge.Start, Edge.End, Color, WorldReference, LineThickness, DisplayHeight);
}

void UUtility::DrawLine(const FVector2D &Start, const FVector2D &End, const FColor Color, const AActor *WorldReference, const float LineThickness, const float DisplayHeight) {
	UWorld *World = WorldReference->GetWorld();
	if(!World)
		return;
	
	DrawDebugLine(World, FVector(Start.X, Start.Y, DisplayHeight), FVector(End.X, End.Y, DisplayHeight), Color, false, -1, 0, LineThickness);
}

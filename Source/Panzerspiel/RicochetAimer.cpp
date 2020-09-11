// All rights reserved @Apfelstrudel Games.


#include "RicochetAimer.h"


#include "CubeObstacle.h"
#include "Kismet/GameplayStatics.h"
#include "TankPawn.h"
#include "DrawDebugHelpers.h"

// Sets default values
ARicochetAimer::ARicochetAimer() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARicochetAimer::BeginPlay() {
	Super::BeginPlay();

	Cubes.Empty();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, ACubeObstacle::StaticClass(), FoundActors);
	for (AActor* Cube : FoundActors)
		Cubes.Add(Cast<ACubeObstacle>(Cube));
}

// Called every frame
void ARicochetAimer::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	const double Start = FPlatformTime::Seconds();

	// TODO: Doing the gathering and intersecting of the edges both in the same loop could improve the performance, the downside is that we then cant use the potentially visible edges for comparison with other tanks.
	if (IsValid(TankPawn) && IsValid(TankPawn2)) {
		Edges1.Empty();
		const FVector2D Tank1Location = FVector2D(TankPawn->GetActorLocation());
		for (ACubeObstacle* Obstacle : Cubes) {
			Edges1.Append(Obstacle->GetPossibleEdges2(Tank1Location));
		}
		Edges1.Sort();
	
		Edges2.Empty();
		const FVector2D Tank2Location = FVector2D(TankPawn2->GetActorLocation());
		for (ACubeObstacle* Obstacle : Cubes) {
			Edges2.Append(Obstacle->GetPossibleEdges2(Tank2Location));
		}
		Edges2.Sort();

		// Only keep edges that are visible from both locations.
		IntersectedEdges = IntersectArrays(Edges1, Edges2);

		// Only keep edges that can reflect the bullet to the target according to their rotation.
		TArray<FObstacleEdge> FilteredEdges;
		for(FObstacleEdge &Edge : IntersectedEdges)
			if(CanBulletEverHitTarget(Edge, Tank1Location, Tank2Location))
				FilteredEdges.Add(Edge);
		// Make sure its empty.
		IntersectedEdges.Empty();

		TArray<FBulletPath> BulletPaths;
		for(const FObstacleEdge &Edge : FilteredEdges)
			RaycastFilter(Edge, Tank1Location, Tank2Location, BulletPaths);

		if(bDebugDrawRaycastCalculation) ShowBulletPaths(BulletPaths);

		const double End = FPlatformTime::Seconds();
		if (bDebugLog) UE_LOG(LogTemp, Warning, TEXT("code executed in %f seconds, found %d edges."), End-Start, BulletPaths.Num());
	}
}

void ARicochetAimer::ShowEdges(TArray<FObstacleEdge> &EdgesToShow) const {
	for (const FObstacleEdge &Edge : EdgesToShow) {
		FVector From = FVector(Edge.Start.X, Edge.Start.Y, DisplayHeight);
		FVector To = FVector(Edge.End.X, Edge.End.Y, DisplayHeight);
		DrawDebugLine(GetWorld(), From, To, FColor::Green, false, -1, 0, LineThickness);
	}
}

TArray<FObstacleEdge> ARicochetAimer::IntersectArrays(TArray<FObstacleEdge> &First, TArray<FObstacleEdge> &Second) {
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

bool ARicochetAimer::CanBulletEverHitTarget(const FObstacleEdge &Edge, const FVector2D &BulletOrigin, const FVector2D &Target) const {
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
	
	if(bDebugDrawEdgeCalculation) {
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
	}
	
	// If the sign from the cross products of both the left with the middle and the right and the middle have a different
	// sign then the target could possibly get hit.
	return (StartTargetDirection ^ MirroredStartDirection) * (EndTargetDirection ^ MirroredEndDirection) < 0;
}

FVector2D ARicochetAimer::MirrorVector(const FVector2D ToMirror, const FVector2D MirrorOrigin, const FVector2D MirrorDirection) {
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


FVector2D ARicochetAimer::MirrorPoint(const FVector2D ToMirror, const FVector2D MirrorOrigin, const FVector2D MirrorDirection) {
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

void ARicochetAimer::RaycastFilter(const FObstacleEdge &Edge, const FVector2D &Origin, const FVector2D &Target, TArray<FBulletPath> &BulletPaths) const {
	UWorld *World = GetWorld();
	if(!World)
		return;
	// Mirror target at the edge.
	const FVector2D EdgeNormal = FVector2D(Edge.End.Y - Edge.Start.Y, -(Edge.End.X - Edge.Start.X));
	FVector2D MirroredTarget = MirrorPoint(Target, Edge.Start, Edge.End - Edge.Start);
	// Do raycast from the origin and check if it hit the edge.
	FHitResult HitResult;
	FCollisionQueryParams Params;
	// TODO: TankPawn should not be hardcoded right here as well as below.
	Params.AddIgnoredActor(TankPawn);
	World->LineTraceSingleByChannel(HitResult, FVector(Origin.X, Origin.Y, RaycastHeight),
		FVector(MirroredTarget.X, MirroredTarget.Y, RaycastHeight), ECollisionChannel::ECC_Camera, Params);
	const FVector2D EdgeDirection = Edge.End - Edge.Start;
	FVector2D HitLocation = FVector2D(HitResult.Location.X, HitResult.Location.Y);
	float CrossProduct = FVector2D::CrossProduct(EdgeDirection, (HitLocation - Edge.Start));
	//if(bDebugLog) UE_LOG(LogTemp, Warning, TEXT("CrossProduct1: %f"), CrossProduct);
	if(FMath::Abs(CrossProduct) > HitThreshold) {
		// We hit something else than the edge.
		return;
	}
	// Store the full PathLength.
	float PathLength = HitResult.Distance;

	// Do raycast from the target and check if it hit the edge.
	Params.ClearIgnoredActors();
	Params.AddIgnoredActor(TankPawn2);
	// We do the raycast to the last hit location and multiply it by two just to make sure it does not stop directly in front of it.
	FVector RaycastOrigin = FVector(Target.X, Target.Y, RaycastHeight);
	FVector RaycastTarget = HitResult.Location + HitResult.Location - RaycastOrigin;
	World->LineTraceSingleByChannel(HitResult, FVector(Target.X, Target.Y, RaycastHeight),
        RaycastTarget, ECollisionChannel::ECC_Camera, Params);
	HitLocation = FVector2D(HitResult.Location.X, HitResult.Location.Y);
	CrossProduct = FVector2D::CrossProduct(EdgeDirection, (HitLocation - Edge.Start));
	//if(bDebugLog) UE_LOG(LogTemp, Warning, TEXT("CrossProduct2: %f"), CrossProduct);
	if(FMath::Abs(CrossProduct) > HitThreshold) {
		// We hit something else than the edge.
		return; 
	}

	UE_LOG(LogTemp, Warning, TEXT("Hit both edges."));
	// We hit both edges.
	PathLength += HitResult.Distance;
	BulletPaths.Add(FBulletPath(FVector(MirroredTarget.X, MirroredTarget.Y, 0) , PathLength));
}

void ARicochetAimer::ShowBulletPaths(TArray<FBulletPath> &BulletPaths) const {
	for(FBulletPath Path : BulletPaths) {
		FVector From = TankPawn->GetActorLocation();
		FVector To = From + (Path.Target - From) / 2;
		DrawDebugLine(GetWorld(), From, To, FColor::Green, false, -1, 0, LineThickness);
	}
}

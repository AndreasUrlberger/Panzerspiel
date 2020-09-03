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

		IntersectedEdges = IntersectArrays(Edges1, Edges2);

		TArray<FObstacleEdge> FilteredEdges;
		for(FObstacleEdge Edge : IntersectedEdges)
			if(CanBulletEverHitTarget(Edge, Tank1Location, Tank2Location))
				FilteredEdges.Add(Edge);

		IntersectedEdges = FilteredEdges;
		// Just to make the inspection in the editor easier.
		IntersectedEdges.Sort();
		if (bDebugDraw) ShowEdges(IntersectedEdges);

		double End = FPlatformTime::Seconds();
		if (bDebugLog) UE_LOG(LogTemp, Warning, TEXT("code executed in %f seconds, found %d edges."), End-Start, IntersectedEdges.Num());
	}
}

void ARicochetAimer::ShowEdges(TArray<FObstacleEdge> EdgesToShow) const {
	for (const FObstacleEdge Edge : EdgesToShow) {
		FVector From = FVector(Edge.Start.X, Edge.Start.Y, DisplayHeight);
		FVector To = FVector(Edge.End.X, Edge.End.Y, DisplayHeight);
		DrawDebugLine(GetWorld(), From, To, FColor::Green, false, -1, 0, LineThickness);
	}
}

TArray<FObstacleEdge> ARicochetAimer::IntersectArrays(TArray<FObstacleEdge> First, TArray<FObstacleEdge> Second) {
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

bool ARicochetAimer::CanBulletEverHitTarget(const FObstacleEdge& Edge, FVector2D BulletOrigin, FVector2D Target) {
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
	const FVector2D TargetDirection = Target - EdgeMiddle;
	// Rotate that vector by 90 degree.
	const FVector2D RotatedTargetDirection = FVector2D(TargetDirection.Y, - TargetDirection.X);
	
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

		// MirrorDirection in cyan.
		DrawDebugLine(GetWorld(), FVector(EdgeMiddle.X, EdgeMiddle.Y, DisplayHeight),
            FVector(EdgeMiddle.X + EdgeNormal.X, EdgeMiddle.Y + EdgeNormal.Y,
                DisplayHeight), FColor::Cyan, false, -1, 0, LineThickness);
	}
	
	// Calculate dot product of both and check the sign of the product.
	return (RotatedTargetDirection | MirroredStartDirection) * (RotatedTargetDirection | MirroredEndDirection) < 0;
}

FVector2D ARicochetAimer::MirrorVector(const FVector2D ToMirror, const FVector2D MirrorOrigin, const FVector2D MirrorDirection) {
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

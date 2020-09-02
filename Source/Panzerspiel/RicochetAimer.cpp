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


	double Start = FPlatformTime::Seconds();

	if (IsValid(TankPawn)) {
		Edges1.Empty();
		const FVector2D Location = FVector2D(TankPawn->GetActorLocation());
		for (ACubeObstacle* Obstacle : Cubes) {
			if (!Obstacle)
			UE_LOG(LogTemp, Warning, TEXT("Obstacle is null"));
			Edges1.Append(Obstacle->GetPossibleEdges2(Location));
		}
		/*if(bDebugDraw)
			ShowEdges(Edges1);*/
		Edges1.Sort();
	}

	if (IsValid(TankPawn2)) {
		Edges2.Empty();
		const FVector2D Location = FVector2D(TankPawn2->GetActorLocation());
		for (ACubeObstacle* Obstacle : Cubes) {
			if (!Obstacle)
			UE_LOG(LogTemp, Warning, TEXT("Obstacle is null"));
			Edges2.Append(Obstacle->GetPossibleEdges2(Location));
		}
		/*if(bDebugDraw)
			ShowEdges(Edges2);*/
		Edges2.Sort();
	}

	// TODO: Create the union of the edges to get the ones that are visible from both tanks.
	IntersectedEdges = IntersectArrays(Edges1, Edges2);
	// Just to make the inspection in the editor easier.
	IntersectedEdges.Sort();
	if (bDebugDraw) ShowEdges(IntersectedEdges);

	double End = FPlatformTime::Seconds();
	if (bDebugLog) UE_LOG(LogTemp, Warning, TEXT("code executed in %f seconds, found %d edges."), End-Start,
	                      IntersectedEdges.Num());
}

void ARicochetAimer::ShowEdges(TArray<FObstacleEdge> EdgesToShow) const {
	for (const FObstacleEdge Edge : EdgesToShow) {
		FVector From = FVector(Edge.Start.X, Edge.Start.Y, DisplayHeight);
		FVector To = FVector(Edge.End.X, Edge.End.Y, DisplayHeight);
		DrawDebugLine(GetWorld(), From, To, FColor::Green, false, -1, 0, LineThickness);
	}
}

TArray<FObstacleEdge> ARicochetAimer::IntersectArrays(TArray<FObstacleEdge> First, TArray<FObstacleEdge> Second) {
	UE_LOG(LogTemp, Warning, TEXT("Called IntersectArrays"));
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
	UE_LOG(LogTemp, Warning, TEXT("Leaves IntersectArrays"));
	return Intersection;
}

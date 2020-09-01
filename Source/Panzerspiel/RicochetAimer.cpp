// All rights reserved @Apfelstrudel Games.


#include "RicochetAimer.h"

#include "CubeObstacle.h"
#include "Kismet/GameplayStatics.h"
#include "TankPawn.h"
#include "DrawDebugHelpers.h"

// Sets default values
ARicochetAimer::ARicochetAimer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ARicochetAimer::BeginPlay()
{
	Super::BeginPlay();

	Cubes.Empty();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(this, ACubeObstacle::StaticClass(), FoundActors);
	for(AActor *Cube : FoundActors)
		Cubes.Add(Cast<ACubeObstacle>(Cube));
}

// Called every frame
void ARicochetAimer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	double Start = FPlatformTime::Seconds();
	
	if(IsValid(TankPawn)) {
		Edges.Empty();
		const FVector2D Location = FVector2D(TankPawn->GetActorLocation());
		for(ACubeObstacle *Obstacle : Cubes) {
			if(!Obstacle)
				UE_LOG(LogTemp, Warning, TEXT("Obstacle is null"));
			Edges.Append(Obstacle->GetPossibleEdges2(Location));
		}
		if(bDebugDraw)
			ShowEdges(Edges);
	}
	
	double End = FPlatformTime::Seconds();
	if(bDebugLog) UE_LOG(LogTemp, Warning, TEXT("code executed in %f seconds, found %d edges."), End-Start, Edges.Num());
}

void ARicochetAimer::ShowEdges(TArray<FObstacleEdge> EdgesToShow) const {
	for(const FObstacleEdge Edge : EdgesToShow) {
		FVector From = FVector(Edge.Start.X, Edge.Start.Y, DisplayHeight);
		FVector To = FVector(Edge.End.X, Edge.End.Y, DisplayHeight);
		DrawDebugLine(GetWorld(), From, To, FColor::Green, false, -1, 0, LineThickness);
	}
}


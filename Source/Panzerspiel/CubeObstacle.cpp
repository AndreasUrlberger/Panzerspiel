// All rights reserved @Apfelstrudel Games.


#include "CubeObstacle.h"

ACubeObstacle::ACubeObstacle() {
	// Populate CornerMarkers.
	TopLeftC = CreateDefaultSubobject<USceneComponent>(TEXT("CornerTopLeft"));
	TopLeftC->SetupAttachment(RootComponent);
	CornerMarkers.Add(TopLeftC);
	TopRightC = CreateDefaultSubobject<USceneComponent>(TEXT("CornerTopRight"));
	TopRightC->SetupAttachment(RootComponent);
	CornerMarkers.Add(TopRightC);
	BottomRightC = CreateDefaultSubobject<USceneComponent>(TEXT("CornerBottomRight"));
	BottomRightC->SetupAttachment(RootComponent);
	CornerMarkers.Add(BottomRightC);
	BottomLeftC = CreateDefaultSubobject<USceneComponent>(TEXT("CornerBottomLeft"));
	BottomLeftC->SetupAttachment(RootComponent);
	CornerMarkers.Add(BottomLeftC);

}

void ACubeObstacle::BeginPlay() {
	Super::BeginPlay();
	
	const FVector2D TopLeft = FVector2D(TopLeftC->GetComponentLocation());
	const FVector2D TopRight = FVector2D(TopRightC->GetComponentLocation());
	const FVector2D BottomRight = FVector2D(BottomRightC->GetComponentLocation());
	const FVector2D BottomLeft = FVector2D(BottomLeftC->GetComponentLocation());

	Edges.Add(NewObject<UObstacleEdge>()->Init(TopLeft, TopRight, this));
	Edges.Add(NewObject<UObstacleEdge>()->Init(TopRight, BottomRight, this));
	Edges.Add(NewObject<UObstacleEdge>()->Init(BottomRight, BottomLeft, this));
	Edges.Add(NewObject<UObstacleEdge>()->Init(BottomLeft, TopLeft, this));
}

TArray<UObstacleEdge*> ACubeObstacle::GetPossibleEdges(const FVector2D &BulletOrigin) const {
	TArray<UObstacleEdge*> PossibleEdges;
	
	for(int32 Index = 0; Index < 4; ++Index) {
		const FVector2D Edge = Edges[Index]->End - Edges[Index]->Start;
		const FVector2D EdgeLocation = Edges[Index]->Start;
		const FVector2D EdgeNormal = FVector2D(Edge.Y, -Edge.X);
		const FVector2D TankDirection = BulletOrigin - EdgeLocation;
		if(FVector2D::DotProduct(TankDirection, EdgeNormal) > 0) {
			PossibleEdges.Add(Edges[Index]);
		}
	}

	return PossibleEdges;
}



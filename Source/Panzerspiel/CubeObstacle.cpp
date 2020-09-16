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
	CornersLocations.Add(FVector2D(TopLeftC->GetComponentLocation()));
	CornersLocations.Add(FVector2D(TopRightC->GetComponentLocation()));
	CornersLocations.Add(FVector2D(BottomRightC->GetComponentLocation()));
	CornersLocations.Add(FVector2D(BottomLeftC->GetComponentLocation()));

	const FVector2D TopLeft = FVector2D(TopLeftC->GetComponentLocation());
	const FVector2D TopRight = FVector2D(TopRightC->GetComponentLocation());
	const FVector2D BottomRight = FVector2D(BottomRightC->GetComponentLocation());
	const FVector2D BottomLeft = FVector2D(BottomLeftC->GetComponentLocation());
	EdgeLocations.Add(TopLeft + (TopRight - TopLeft)/2);
	EdgeLocations.Add(TopRight + (BottomRight - TopRight)/2);
	EdgeLocations.Add(BottomRight + (BottomLeft - BottomRight)/2);
	EdgeLocations.Add(BottomLeft + (TopLeft - BottomLeft)/2);
}

TArray<UObstacleEdge*> ACubeObstacle::GetPossibleEdges(FVector2D BulletOrigin) {
	// Copy CornersLocations since we're about to modify them.
	TArray<FVector2D> Corners;
	Corners.Append(CornersLocations);

	// They are not really the most left or right, it just helps to imagine them like that.
	int32 LeftMostIndex = 0;
	int32 RightMostIndex = 0;
	// This method is specifically and only designed for this cube class so it will always have four corners.
	for(int32 Index = 0; Index < 4; ++Index) {
		const FVector2D Direction = Corners[Index] - BulletOrigin;
		const FVector2D DirectionLeft = Corners[LeftMostIndex] - BulletOrigin;
		// Rotate 90 degree to make a comparision possible.
		const FVector2D DirectionLeftRotated = FVector2D(DirectionLeft.Y, -DirectionLeft.X);
		const FVector2D DirectionRight = Corners[RightMostIndex] - BulletOrigin;
		// Same as for the Left vector.
		const FVector2D DirectionRightRotated = FVector2D(DirectionRight.Y, -DirectionRight.X);

		if(FVector2D::DotProduct(DirectionLeftRotated, Direction) > 0) {
			// More left than the leftmost.
			LeftMostIndex = Index;
		}else if(FVector2D::DotProduct(DirectionRightRotated, Direction) < 0 ){
			// More than the rightMost.
			RightMostIndex = Index;
		}
	}
	
	// Now we have the left- and rightmost corner.
	const FVector2D BorderVector = Corners[LeftMostIndex] - Corners[RightMostIndex];
	const FVector2D RotatedBorder = FVector2D(BorderVector.Y, -BorderVector.X);
	// Add all possible edges.
	TArray<UObstacleEdge*> PossibleEdges;
	UObstacleEdge* Edge1 = NewObject<UObstacleEdge>();
	PossibleEdges.Add(Edge1->Init(Corners[0], Corners[1], this));
	UObstacleEdge* Edge2 = NewObject<UObstacleEdge>();
	PossibleEdges.Add(Edge2->Init(Corners[1], Corners[2], this));
	UObstacleEdge* Edge3 = NewObject<UObstacleEdge>();
	PossibleEdges.Add(Edge3->Init(Corners[2], Corners[3], this));
	UObstacleEdge* Edge4 = NewObject<UObstacleEdge>();
	PossibleEdges.Add(Edge4->Init(Corners[3], Corners[0], this));

	// Find which corners are not visible and thus are to be removed.
	for(int32 Index = 0; Index < 4; ++Index) {
		if(Index != LeftMostIndex && Index != RightMostIndex) {
			FVector2D Direction = Corners[Index] - FVector2D(GetActorLocation());
			if(FVector2D::DotProduct(RotatedBorder, Direction) < 0) {
				FVector2D Corner = Corners[Index];
				PossibleEdges.RemoveAll([Corner](UObstacleEdge *n){ return n->Start == Corner || n->End == Corner; });
			}
		}
	}
	return PossibleEdges;
}

TArray<UObstacleEdge*> ACubeObstacle::GetPossibleEdges2(const FVector2D &BulletOrigin) const {
	TArray<UObstacleEdge*> PossibleEdges;
	// First Edge.
	FVector2D Edge = CornersLocations[1] - CornersLocations[0];
	FVector2D EdgeLocation = EdgeLocations[0];
	FVector2D EdgeNormal = FVector2D(Edge.Y, -Edge.X);
	FVector2D TankDirection = BulletOrigin - EdgeLocation;
	if(FVector2D::DotProduct(TankDirection, EdgeNormal) > 0) {
		UObstacleEdge* NewEdge = NewObject<UObstacleEdge>();
		PossibleEdges.Add(NewEdge->Init(CornersLocations[0], CornersLocations[1], this));
	}

	// Second Edge.
	Edge = CornersLocations[2] - CornersLocations[1];
	EdgeLocation = EdgeLocations[1];
	EdgeNormal = FVector2D(Edge.Y, -Edge.X);
	TankDirection = BulletOrigin - EdgeLocation;
	if(FVector2D::DotProduct(TankDirection, EdgeNormal) > 0) {
		UObstacleEdge* NewEdge = NewObject<UObstacleEdge>();
		PossibleEdges.Add(NewEdge->Init(CornersLocations[1], CornersLocations[2], this));
	}

	// Third Edge.
	Edge = CornersLocations[3] - CornersLocations[2];
	EdgeLocation = EdgeLocations[2];
	EdgeNormal = FVector2D(Edge.Y, -Edge.X);
	TankDirection = BulletOrigin - EdgeLocation;
	if(FVector2D::DotProduct(TankDirection, EdgeNormal) > 0) {
		UObstacleEdge* NewEdge = NewObject<UObstacleEdge>();
		PossibleEdges.Add(NewEdge->Init(CornersLocations[2], CornersLocations[3], this));
	}

	// Fourth Edge.
	Edge = CornersLocations[0] - CornersLocations[3];
	EdgeLocation = EdgeLocations[3];
	EdgeNormal = FVector2D(Edge.Y, -Edge.X);
	TankDirection = BulletOrigin - EdgeLocation;
	if(FVector2D::DotProduct(TankDirection, EdgeNormal) > 0) {
		UObstacleEdge* NewEdge = NewObject<UObstacleEdge>();
		PossibleEdges.Add(NewEdge->Init(CornersLocations[3], CornersLocations[0], this));
	}

	return PossibleEdges;
}



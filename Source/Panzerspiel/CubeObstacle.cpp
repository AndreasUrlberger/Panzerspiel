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

	CornersLocations.Add(FVector2D(TopLeftC->GetComponentLocation()));
	CornersLocations.Add(FVector2D(TopRightC->GetComponentLocation()));
	CornersLocations.Add(FVector2D(BottomRightC->GetComponentLocation()));
	CornersLocations.Add(FVector2D(BottomLeftC->GetComponentLocation()));
	
}

void ACubeObstacle::BeginPlay() {
	Super::BeginPlay();
}

TArray<FObstacleEdge> ACubeObstacle::GetPossibleEdges(FVector2D BulletOrigin) {
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
		const FVector2D DirectionRightRotated = FVector2D(DirectionRight.Y, -Direction.X);
		
		if(FVector2D::DotProduct(DirectionLeftRotated, Direction) < 0) {
			// More left than the leftmost.
			LeftMostIndex = Index;
		}else if(FVector2D::DotProduct(DirectionRightRotated, Direction) > 0 ){
			// More than the rightMost.
			RightMostIndex = Index;
		}
	}

	// Now we have the left- and rightmost corner.
	const FVector2D BorderVector = Corners[LeftMostIndex] - Corners[RightMostIndex];
	FVector2D RotatedBorder = FVector2D(BorderVector.Y, -BorderVector.X);

	// Find which corners are not visible and thus are to be removed.
	TArray<int32> ToRemove;
	for(int32 Index = 0; Index < 4; ++Index) {
		if(Index != LeftMostIndex && Index != RightMostIndex) {
			FVector2D Direction = Corners[Index] - FVector2D(GetActorLocation());
			if(FVector2D::DotProduct(RotatedBorder, Direction) > 0)
				ToRemove.Add(Index);
		}
	}
	for(int32 ToRemoveAt : ToRemove)
		Corners.RemoveAt(ToRemoveAt);

	// Add all possible edges.
	TArray<FObstacleEdge> PossibleEdges;
	for(int32 Index = 1; Index < Corners.Num(); ++Index)
		PossibleEdges.Add(FObstacleEdge(Corners[Index - 1], Corners[Index] - Corners[Index - 1]));
	
	return PossibleEdges;
}

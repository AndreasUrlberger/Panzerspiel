// All rights reserved @Apfelstrudel Games.


#include "DoubleRicochetTest.h"
#include "TankPawn.h"
#include "CubeObstacle.h"
#include "DrawDebugHelpers.h"
#include "StaticMeshAttributes.h"

// Sets default values
ADoubleRicochetTest::ADoubleRicochetTest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADoubleRicochetTest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoubleRicochetTest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!Tank1 || !Tank2 || !Cube1 || !Cube2)
		return;

	const FVector TLocation1 = FVector(Tank1->GetActorLocation().X, Tank1->GetActorLocation().Y, DisplayHeight);
	const FVector TLocation2 = FVector(Tank2->GetActorLocation().X, Tank2->GetActorLocation().Y, DisplayHeight);
	const FVector Cube1EdgeCorner1 = Cube1->GetCornerMarkers()[Cube1EdgeIndex]->GetComponentLocation();
	const FVector Cube1EdgeCorner2 = Cube1->GetCornerMarkers()[(Cube1EdgeIndex + 1) % 4]->GetComponentLocation();
	const FObstacleEdge Edge1 = FObstacleEdge(FVector2D(Cube1EdgeCorner1.X, Cube1EdgeCorner1.Y), FVector2D(Cube1EdgeCorner2.X, Cube1EdgeCorner2.Y));
	const FVector Edge1Middle = FVector(Edge1.Start.X + (Edge1.End.X - Edge1.Start.X)/2, Edge1.Start.Y + (Edge1.End.Y - Edge1.Start.Y)/2, DisplayHeight);
	const FVector Cube2EdgeCorner1 = Cube2->GetCornerMarkers()[Cube2EdgeIndex]->GetComponentLocation();
	const FVector Cube2EdgeCorner2 = Cube2->GetCornerMarkers()[(Cube2EdgeIndex + 1) % 4]->GetComponentLocation();
	const FObstacleEdge Edge2 = FObstacleEdge(FVector2D(Cube2EdgeCorner1.X, Cube2EdgeCorner1.Y), FVector2D(Cube2EdgeCorner2.X, Cube2EdgeCorner2.Y));
	const FVector Edge2Middle = FVector(Edge2.Start.X + (Edge2.End.X - Edge2.Start.X)/2, Edge2.Start.Y + (Edge2.End.Y - Edge2.Start.Y)/2, DisplayHeight);

	const FVector Tank1Normal = Tank1->GetActorForwardVector();
	const FVector Tank2Normal = Tank2->GetActorForwardVector();
	const FVector Edge1Normal = FVector(Edge1.End.Y - Edge1.Start.Y, -(Edge1.End.X - Edge1.Start.X), 0).GetUnsafeNormal();
	const FVector Edge2Normal = FVector(Edge2.End.Y - Edge2.Start.Y, -(Edge2.End.X - Edge2.Start.X), 0).GetUnsafeNormal();

	DrawDebugLine(GetWorld(), TLocation1, TLocation2, FColor::Orange, false, -1, 0, LineThickness);
	DrawDebugLine(GetWorld(), TLocation1, Edge1Middle, FColor::Orange, false, -1, 0, LineThickness);
	DrawDebugLine(GetWorld(), TLocation2, Edge2Middle, FColor::Orange, false, -1, 0, LineThickness);
	DrawDebugLine(GetWorld(), Edge1Middle, Edge2Middle, FColor::Orange, false, -1, 0, LineThickness);
	DrawDebugLine(GetWorld(), Edge1Middle, Edge1Middle + Edge1Normal * 300, FColor::Blue, false, -1, 0, LineThickness);
	DrawDebugLine(GetWorld(), Edge2Middle, Edge2Middle + Edge2Normal * 300, FColor::Blue, false, -1, 0, LineThickness);
	DrawDebugLine(GetWorld(), TLocation1, TLocation1 + Tank1Normal * 300, FColor::Blue, false, -1, 0, LineThickness);
	DrawDebugLine(GetWorld(), TLocation2, TLocation2 + Tank2Normal * 300, FColor::Blue, false, -1, 0, LineThickness);

	// Tank1 Cube1 Cube2.
	const FVector Cube1Tank1 = TLocation1 - Edge1Middle;
	const FVector Cube1Cube2 = Edge2Middle - Edge1Middle;
	float Scalar = Cube1Tank1.GetUnsafeNormal() | Cube1Cube2.GetUnsafeNormal();
	float Cross = (Cube1Tank1 ^ Cube1Cube2).Z;
	UE_LOG(LogTemp, Warning, TEXT("1. Angle: %3.3f, Scalar: %3.3f, CrossProduct: %3.3f"), GetAngleBetween(Cube1Tank1, Cube1Cube2), Scalar, Cross);

	// Cube1 Cube2 Tank2.
	const FVector Cube2Tank2 = TLocation2 - Edge2Middle;
	const FVector Cube2Cube1 = -Cube1Cube2;
	Scalar = Cube2Tank2.GetUnsafeNormal() | Cube2Cube1.GetUnsafeNormal();
	Cross = (Cube2Tank2 ^ Cube2Cube1).Z;
	UE_LOG(LogTemp, Warning, TEXT("2. Angle: %3.3f, Scalar: %3.3f, CrossProduct: %3.3f"), GetAngleBetween(Cube2Tank2, Cube2Cube1), Scalar, Cross);

	// Cube2 Tank2 Tank1.
	const FVector Tank2Cube2 = -Cube2Tank2;
	const FVector Tank2Tank1 = TLocation1 - TLocation2;
	Scalar = Tank2Cube2.GetUnsafeNormal() | Tank2Tank1.GetUnsafeNormal();
	Cross = (Tank2Cube2 ^ Tank2Tank1).Z;
	UE_LOG(LogTemp, Warning, TEXT("3. Angle: %3.3f, Scalar: %3.3f, CrossProduct: %3.3f"), GetAngleBetween(Tank2Cube2, Tank2Tank1), Scalar, Cross);

	// Tank2 Tank1 Cube1
	const FVector Tank1Tank2 = -Tank2Tank1;
	const FVector Tank1Cube1 = -Cube1Tank1;
	Scalar = Tank1Tank2.GetUnsafeNormal() | Tank1Cube1.GetUnsafeNormal();
	Cross = (Tank1Tank2 ^ Tank1Cube1).Z;
	UE_LOG(LogTemp, Warning, TEXT("4. Angle: %3.3f, Scalar: %3.3f, CrossProduct: %3.3f"), GetAngleBetween(Tank1Tank2, Tank1Cube1), Scalar, Cross);
}

float ADoubleRicochetTest::GetAngleBetween(const FVector Vector1, const FVector Vector2) {
	const float DotProduct = Vector1.GetUnsafeNormal() | Vector2.GetUnsafeNormal();
	return FMath::Acos(DotProduct) * 180/PI;
}


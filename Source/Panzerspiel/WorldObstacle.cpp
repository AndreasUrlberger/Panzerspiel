// All rights reserved @Apfelstrudel Games.


#include "WorldObstacle.h"

UObstacleEdge::UObstacleEdge() {
	static int32 StaticIdCounter = 1;
	ID = StaticIdCounter;
	++StaticIdCounter;
}

UObstacleEdge* UObstacleEdge::Init(FVector2D NewStart, FVector2D NewEnd, const AActor* NewParent) {
	Start = NewStart;
	End = NewEnd;
	Parent = NewParent;
	return this;
}

UObstacleEdge* UObstacleEdge::Copy(const UObstacleEdge* Other) {
	Start = Other->Start;
	End = Other->End;
	Parent = Other->Parent;
	ID = Other->ID;
	return this;
}

// FObstacleEdge operators.
bool UObstacleEdge::Equals(const UObstacleEdge& Other) const {
	return ID == Other.ID;
}

bool UObstacleEdge::operator<(const UObstacleEdge& Other) const {
	return ID < Other.ID;
}

/**
 *	Currently now working as intended, use with care.
 */
bool UObstacleEdge::operator<=(const UObstacleEdge& Other) const {
	return ID <= Other.ID;
}

/**
*	Currently now working as intended, use with care.
*/
bool UObstacleEdge::operator>(const UObstacleEdge& Other) const {
	return ID > Other.ID;
}

/**
*	Currently now working as intended, use with care.
*/
bool UObstacleEdge::operator>=(const UObstacleEdge& Other) const {
	return ID >= Other.ID;
}

bool UObstacleEdge::operator==(const UObstacleEdge& Other) const
{
	return ID == Other.ID;
}

bool UObstacleEdge::operator!=(const UObstacleEdge& Other) const
{
	return ID != Other.ID;
}

FString UObstacleEdge::ToString() const
{
	return FString::Printf(TEXT("{Start=%s End=%s}"), *Start.ToString(), *End.ToString());
}

// Sets default values
AWorldObstacle::AWorldObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComp"));
	SetRootComponent(RootSceneComp);
}

TArray<USceneComponent*> AWorldObstacle::GetCornerMarkers() {
	return CornerMarkers;
}

TArray<UObstacleEdge*> AWorldObstacle::GetPossibleEdges(const FVector2D &BulletOrigin) const {
	TArray<UObstacleEdge*> Empty;
	return Empty;
}

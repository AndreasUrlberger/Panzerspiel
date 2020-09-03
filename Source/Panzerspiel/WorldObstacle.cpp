// All rights reserved @Apfelstrudel Games.


#include "WorldObstacle.h"

// FObstacleEdge operators.
FORCEINLINE bool FObstacleEdge::Equals(const FObstacleEdge& OtherEdge) const {
	return this->Start.Equals(OtherEdge.Start) && this->End.Equals(OtherEdge.End);
}

FORCEINLINE bool FObstacleEdge::operator<(const FObstacleEdge& Other) const {
	if(Start.X < Other.Start.X)
		return true;
	else if (Start.X > Other.Start.X)
		return false;
	// Start.X and Other.Start.X must be equal so we now check Start.Y.
	else if(Start.Y < Other.Start.Y)
		return true;
	else if (Start.Y > Other.Start.Y)
		return false;
	// Start and Other.Start must be equal so we now check End.
	else if(End.X < Other.End.X)
		return true;
	else if (End.X > Other.End.X)
		return false;
	// Start.X and Other.Start.X must be equal so we now check Start.Y.
	else if(End.Y < Other.End.Y)
		return true;
	else if (End.Y > Other.End.Y)
		return false;
	// This and Other are equal.
	else
		return false;
}

/**
 *	Currently now working as intended, use with care.
 */
FORCEINLINE bool FObstacleEdge::operator<=(const FObstacleEdge& Other) const {
	if(this->Start <= Other.Start) {
		return true;
	}else if(this->Start > Other.Start){
		return false;
	}else {
		return this->End <= Other.End;
	}
}

/**
*	Currently now working as intended, use with care.
*/
FORCEINLINE bool FObstacleEdge::operator>(const FObstacleEdge& Other) const {
	if(this->Start > Other.Start) {
		return true;
	}else if(this->Start < Other.Start){
		return false;
	}else {
		return this->End > Other.End;
	}
}

/**
*	Currently now working as intended, use with care.
*/
FORCEINLINE bool FObstacleEdge::operator>=(const FObstacleEdge& Other) const {
	if(this->Start >= Other.Start) {
		return true;
	}else if(this->Start < Other.Start){
		return false;
	}else {
		return this->End >= Other.End;
	}
}

FORCEINLINE bool FObstacleEdge::operator==(const FObstacleEdge& Other) const
{
	return this->Start==Other.Start && this->End==Other.End;
}

FORCEINLINE bool FObstacleEdge::operator!=(const FObstacleEdge& Other) const
{
	return this->Start!=Other.Start && this->End!=Other.End;
}

FORCEINLINE FString FObstacleEdge::ToString() const
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

TArray<FObstacleEdge> AWorldObstacle::GetPossibleEdges(FVector2D BulletOrigin) {
	TArray<FObstacleEdge> Empty;
	return Empty;
}

TArray<FObstacleEdge> AWorldObstacle::GetPossibleEdges2(FVector2D BulletOrigin) {
	TArray<FObstacleEdge> Empty;
	return Empty;
}
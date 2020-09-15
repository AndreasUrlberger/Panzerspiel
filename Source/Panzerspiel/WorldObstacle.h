// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldObstacle.generated.h"

USTRUCT(Immutable)
struct PANZERSPIEL_API FObstacleEdge {

	GENERATED_USTRUCT_BODY()
public:
    FObstacleEdge(FVector2D NewStart = FVector2D::ZeroVector, FVector2D NewEnd = FVector2D::ZeroVector, const AActor *NewParent = nullptr)
	: Start(NewStart), End(NewEnd), Parent(NewParent) { }

	UPROPERTY()
	FVector2D Start;

	UPROPERTY()
	FVector2D End;

	UPROPERTY()
	const AActor *Parent;

	bool Equals(const FObstacleEdge& OtherEdge) const;

	bool operator<(const FObstacleEdge& Other) const;
		
	bool operator<=(const FObstacleEdge& Other) const;
	
	bool operator>(const FObstacleEdge& Other) const;
	
	bool operator>=(const FObstacleEdge& Other) const;
	
	bool operator==(const FObstacleEdge& Other) const;
	
	bool operator!=(const FObstacleEdge& Other) const;

	FString ToString() const;
};


UCLASS()
class PANZERSPIEL_API AWorldObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldObstacle();
	
	UFUNCTION()
	TArray<USceneComponent*> GetCornerMarkers();

protected:
	UPROPERTY(EditAnywhere, Category="Corners")
	TArray<USceneComponent*> CornerMarkers;

	UPROPERTY()
	USceneComponent *RootSceneComp;

public:
	UFUNCTION()
	virtual TArray<FObstacleEdge> GetPossibleEdges(FVector2D BulletOrigin);

	UFUNCTION()
	virtual TArray<FObstacleEdge> GetPossibleEdges2(const FVector2D &BulletOrigin) const;

};

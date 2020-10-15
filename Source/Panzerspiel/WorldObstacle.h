// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldObstacle.generated.h"

UCLASS()
class PANZERSPIEL_API UObstacleEdge : public UObject{

	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleInstanceOnly, Category="Debug")
	int32 ID;
	
public:
	UObstacleEdge();
	
    /*UObstacleEdge(FVector2D NewStart = FVector2D::ZeroVector, FVector2D NewEnd = FVector2D::ZeroVector, const AActor *NewParent = nullptr)
	: Start(NewStart), End(NewEnd), Parent(NewParent) { }*/

	UObstacleEdge* Init(FVector2D NewStart = FVector2D::ZeroVector, FVector2D NewEnd = FVector2D::ZeroVector, const AActor *NewParent = nullptr);

	UObstacleEdge* Copy(const UObstacleEdge* Other);

	UPROPERTY()
	FVector2D Start;

	UPROPERTY()
	FVector2D End;

	UPROPERTY()
	const AActor *Parent;

	bool Equals(const UObstacleEdge& Other) const;

	bool operator<(const UObstacleEdge& Other) const;
		
	bool operator<=(const UObstacleEdge& Other) const;
	
	bool operator>(const UObstacleEdge& Other) const;
	
	bool operator>=(const UObstacleEdge& Other) const;
	
	bool operator==(const UObstacleEdge& Other) const;
	
	bool operator!=(const UObstacleEdge& Other) const;

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
	virtual TArray<UObstacleEdge*> GetPossibleEdges(const FVector2D &BulletOrigin) const;

};

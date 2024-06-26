// Copyright Apfelstrudel Games - All Rights Reserved

#pragma once


#include "CoreMinimal.h"

#include "WorldObstacle.h"
#include "GameFramework/GameModeBase.h"
#include "BulletPath.h"

#include "PanzerspielGameModeBase.generated.h"


USTRUCT(Immutable)
struct PANZERSPIEL_API FEdgeArray {

	GENERATED_USTRUCT_BODY()
public:
	FEdgeArray(TArray<UObstacleEdge*> NewEdges = TArray<UObstacleEdge*>()) : Edges(NewEdges) { }

	UPROPERTY()
	TArray<UObstacleEdge*> Edges;
};

/**
 * 
 */
UCLASS()
class PANZERSPIEL_API APanzerspielGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

private:
	
	UPROPERTY(VisibleAnywhere, Category="Players")
	TArray<APlayerController*> PlayerControllers;

	UPROPERTY(EditAnywhere, Category="Debug")
	bool DebugLog = false;

	UPROPERTY(VisibleInstanceOnly, Category="Players")
	TArray<class ATankPawn*> PlayerPawns;

	// All Edges must be sorted. No UPROPERTY because it is not allowed for nested containers.
	UPROPERTY()
	TMap<const AActor*, FEdgeArray> PlayersEdges;

	UPROPERTY(VisibleInstanceOnly, Category="Shooting Calculation")
	TArray<const AWorldObstacle*> Obstacles;

	/*UPROPERTY(EditAnywhere, Category="Players")
	int8 MaxPlayers = 1;*/
	// Functions

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugLog = false;
	
	UPROPERTY(EditAnywhere, Category="Shooting Calculation")
	float RaycastHeight = 30;

	// For IsPointOnLine comparision.
	UPROPERTY(EditAnywhere, Category="Shooting Calculation")
	float OnLineThreshold = 1;

	UPROPERTY(EditAnywhere, Category="Shooting Calculation")
	float BulletRadius = 15;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

private:
	APanzerspielGameModeBase();

	UFUNCTION()
	void InvalidateEdges();
	
	UFUNCTION()
    void GetAllPlayerEdges();

	UFUNCTION()
    void PopulateObstacles();

	UFUNCTION()
	TArray<UObstacleEdge*>& GetPlayersEdges(const AActor *TankPawn);

public:

	UFUNCTION()
	void AddWorldObstacle(const AWorldObstacle *Obstacle);

	UFUNCTION()
	void RemoveWorldObstacle(const AWorldObstacle *Obstacle);
	
	UFUNCTION(BlueprintCallable, Category="PlayerPawns")
	TArray<ATankPawn*> GetTankPawns() const;

	UFUNCTION(BlueprintCallable, Category="PlayerPawns")
	TArray<ATankPawn*> GetTankPawnsByTeam(int32 TeamIndex);

	UFUNCTION(BlueprintCallable, Category="PlayerPawns")
	TArray<ATankPawn*> GetTankPawnsByTeams(TArray<int32> TeamIndices);

	UFUNCTION(BlueprintCallable, Category="PlayerPawns")
	TArray<ATankPawn*> GetTankPawnNotInTeam(int32 TeamIndex);

	UFUNCTION()
	void TankPawnRemoveSelf(ATankPawn *Self);

	UFUNCTION()
	void TankPawnRegisterSelf(ATankPawn *Self);

	// Must be called before a LevelStream to remove the old TankPawn references.
	UFUNCTION(BlueprintCallable)
	void ClearTankPawns();

	UFUNCTION()
	bool FindDirectPath(FBulletPath &BulletPath, const AActor *Origin, const FVector& OriginLocation, const AActor *Target);

	UFUNCTION()
	bool FindSingleRicochetPath(TArray<struct FBulletPath> &BulletPath, const AActor *Origin, const FVector& OriginLocation,
		const TArray<UObstacleEdge*> &OriginEdges, const AActor *Target, const TArray<UObstacleEdge*> &TargetEdges);

	UFUNCTION()
	bool FindDoubleRicochetPath(const AActor *Origin, const FVector& OriginLocation, const TArray<UObstacleEdge*> &OriginEdges,
    const AActor *Target, const TArray<UObstacleEdge*> &TargetEdges, TArray<FBulletPath> &BulletPaths);

	UFUNCTION(BlueprintCallable)
	bool GetDirectPath(const AActor* Origin, const FVector &OriginLocation, const AActor* Target, FVector &OutTargetLocation);

	UFUNCTION(BlueprintCallable)
	bool GetShortestSingleRicochet(const AActor* Origin, const FVector &OriginLocation, const AActor* Target, FVector &OutTargetLocation);

	UFUNCTION(BlueprintCallable)
	bool GetShortestDoubleRicochet(const AActor* Origin, const FVector &OriginLocation, const AActor* Target, FVector &OutTargetLocation);

	// This gets called whenever a level unloading occurs so that the GameMode can act accordingly.
	UFUNCTION(BlueprintCallable)
	void LevelStreamingBegan();

	// This gets called whenever a level loading finished so that the GameMode can act accordingly.
	UFUNCTION(BlueprintCallable)
    void LevelStreamingEnded();
};

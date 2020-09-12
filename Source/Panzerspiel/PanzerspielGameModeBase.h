// Copyright Apfelstrudel Games - All Rights Reserved

#pragma once


#include "CoreMinimal.h"

#include "WorldObstacle.h"
#include "GameFramework/GameModeBase.h"
#include "BulletPath.h"

#include "PanzerspielGameModeBase.generated.h"

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
	TMap<ATankPawn*, TArray<FObstacleEdge>> PlayersEdges;

	UPROPERTY(VisibleInstanceOnly, Category="Shooting Calculation")
	TArray<AWorldObstacle*> Obstacles;

	/*UPROPERTY(EditAnywhere, Category="Players")
	int8 MaxPlayers = 1;*/
	// Functions

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bDebugLog = false;
	
	UPROPERTY(EditAnywhere, Category="Shooting Calculation")
	float RaycastHeight = 30;

	// For single ricochet.
	UPROPERTY(EditAnywhere, Category="Shooting Calculation")
	float HitThreshold = 25;

	// For double ricochet. Actually uses the squared value to compare.
	UPROPERTY(EditAnywhere, Category="Shooting Calculation")
	float DistanceThreshold = 5;

protected:
	virtual void BeginPlay() override;

private:
	APanzerspielGameModeBase();

public:
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
	void GetAllPlayerEdges();

	UFUNCTION()
	void PopulateObstacles();

	UFUNCTION()
	static bool FindDirectPath(FBulletPath &BulletPath, const AActor *Origin, const AActor *Target);

	UFUNCTION()
	bool FindSingleRicochetPath(TArray<struct FBulletPath> &BulletPath, const AActor *Origin, const TArray<FObstacleEdge> &OriginEdges,
    const AActor *Target, const TArray<FObstacleEdge> &TargetEdges);

	UFUNCTION()
	bool FindDoubleRicochetPath(const AActor *Origin, const TArray<FObstacleEdge> &OriginEdges,
    const AActor *Target, const TArray<FObstacleEdge> &TargetEdges, TArray<FBulletPath> &BulletPaths);
};

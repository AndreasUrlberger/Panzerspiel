// Copyright Apfelstrudel Games - All Rights Reserved

#pragma once


#include "CoreMinimal.h"

#include "WorldObstacle.h"
#include "GameFramework/GameModeBase.h"

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

	TMap<ATankPawn*, TArray<FObstacleEdge>> PlayersEdges;

	UPROPERTY(VisibleInstanceOnly, Category="Shooting Calculation")
	TArray<AWorldObstacle*> Obstacles;

	/*UPROPERTY(EditAnywhere, Category="Players")
	int8 MaxPlayers = 1;*/
	// Functions

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
};

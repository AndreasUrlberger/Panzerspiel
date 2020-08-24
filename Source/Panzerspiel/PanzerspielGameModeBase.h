// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "PanzerspielGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PANZERSPIEL_API APanzerspielGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category="Players")
	TArray<APlayerController*> PlayerControllers;

	UPROPERTY(EditAnywhere, Category="Debug")
	bool DebugLog = false;

	UPROPERTY()
	TArray<class ATankPawn*> PlayerPawns;

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
};

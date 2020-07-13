// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TankPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PANZERSPIEL_API ATankPlayerController : public APlayerController
{
	GENERATED_BODY()

	ATankPlayerController();

private:
	UPROPERTY(EditAnywhere, Category = "Spawning")
    TSubclassOf<AActor> CrosshairToSpawn;

	UPROPERTY(VisibleAnywhere)
	AActor* Crosshair;

	UPROPERTY(EditAnywhere)
	float CrosshairHeight = 10;

	UPROPERTY(EditAnywhere)
	int32 CollisionChannelIndex;

public:

	void Tick(float DeltaSeconds) override;

	void BeginPlay() override;
};

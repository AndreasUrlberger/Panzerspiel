// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "../TankPawn.h"
#include "RotaryAITank.generated.h"

/**
 * 
 */
UCLASS()
class PANZERSPIEL_API ARotaryAITank : public ATankPawn {
	GENERATED_BODY()

// Variables
private:
	// Measured in seconds.
	UPROPERTY(EditAnywhere, Category="Combat")
	float RandomShootDelay = 3;

	// Measured in seconds.
	UPROPERTY(EditAnywhere, Category="Combat")
	float MinShootDelay = 0.3;

	UPROPERTY(EditAnywhere, Category="Combat")
	bool FireMode = false;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	float TimeTillNextShot = 0;

	UPROPERTY(EditAnywhere, Category="Combat")
	float TowerRotationSpeed = 10;

	UPROPERTY(EditAnywhere, Category="Combat")
	float TraceDistance = 5000;

	// Functions
private:
	virtual void MoveRight(float AxisValue) override;
	
	UFUNCTION()
	bool ShootIfPossible(float DeltaTime);

	UFUNCTION()
	bool CheckDirectLOS(FVector& OutTarget);

	UFUNCTION()
	bool CheckSingleRicochetLOS(FVector& OutTarget);

	UFUNCTION()
	bool CheckDoubleRicochetLOS(FVector& OutTarget);

public:
	ARotaryAITank();

	// Called every frame.
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetFireMode(bool DoesFire);
};

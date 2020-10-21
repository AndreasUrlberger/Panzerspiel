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

	UPROPERTY(EditAnywhere, Category="Combat", meta=(UIMin = 0.0, UIMax = 180))
	float AngleLimit = 30;

	UPROPERTY()
	float LastRotValue = 0;

	UPROPERTY()
	float CurrentRotTime = 0;

	// Functions
private:
	virtual void MoveRight(float AxisValue) override;
	
	UFUNCTION()
	bool RotAndShoot();

	UFUNCTION()
	bool CheckDirectLOS(FVector& OutTarget) const;

	UFUNCTION()
	bool CheckSingleRicochetLOS(FVector& OutTarget) const;

	UFUNCTION()
	bool CheckDoubleRicochetLOS(FVector& OutTarget) const;

	UFUNCTION()
	// Returns the current tower rotation.
	float GetRotValue();

public:
	ARotaryAITank();

	// Called every frame.
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetFireMode(bool DoesFire);
};

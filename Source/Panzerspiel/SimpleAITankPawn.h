// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "TankPawn.h"
#include "SimpleAITankPawn.generated.h"

/**
 * 
 */
UCLASS()
class PANZERSPIEL_API ASimpleAITankPawn : public ATankPawn {
	GENERATED_BODY()

// Variables
private:
	UPROPERTY(EditAnywhere, Category="AI")
	float TraceRange;

	UPROPERTY(VisibleAnywhere, Category="AI")
	TArray<float> Distances;

	// Sensors
	UPROPERTY(VisibleAnywhere, Category="AI")
	TArray<class UArrowComponent*> Sensors;

	UPROPERTY(VisibleAnywhere, Category="AI")
	USceneComponent* SensorsRoot;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UArrowComponent* SensorLeft;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UArrowComponent* SensorHalfLeft;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UArrowComponent* SensorFront;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UArrowComponent* SensorHalfRight;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UArrowComponent* SensorRight;

	UPROPERTY()
	bool FollowingPathPoints;

	UPROPERTY()
	class UBTTask_SimpleTankMoveTo* CallingTask;

	UPROPERTY(EditAnywhere, Category="AI")
	TArray<FVector> PathPoints;

	UPROPERTY()
	int32 CurrentPathPoint = 0;

	// The radius at which we reached the target.
	UPROPERTY(EditAnywhere, Category="AI")
	int32 ReachRadius = 5;

	UPROPERTY(VisibleAnywhere, Category="Movement")
	FVector FakeVelocity;

	UPROPERTY(EditAnywhere, Category="CollisionAvoidance")
	float VelocityImpact = 0.25;

	UPROPERTY(EditAnywhere, Category="CollisionAvoidance")
	TArray<float> AvoidDistances;

	UPROPERTY(EditAnywhere, Category="CollisionAvoidance")
	float AvoidStrength = 1;

	UPROPERTY()
	AActor *LockOnActor;

	// Measured in seconds.
	UPROPERTY(EditAnywhere, Category="Combat")
	float RandomShootDelay = 3;

	// Measured in seconds.
	UPROPERTY(EditAnywhere, Category="Combat")
	float MinShootDelay = 0.3;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	bool FireMode = false;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	float TimeTillNextShot = 0;

	// Functions
	virtual void MoveRight(float AxisValue) override;

	UFUNCTION()
	FVector GetAvoidVector();

	UFUNCTION()
	FVector UpdateMovement(float DeltaTime, const FVector DesiredDeltaMove);

public:
	ASimpleAITankPawn();

	UFUNCTION()
	void NavigationTrace();

	// Called every frame.
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool FollowPathPoints(UBTTask_SimpleTankMoveTo* Task, TArray<FVector> Points);

	// To free the lock mode you can just pass a nullptr.
	UFUNCTION(BlueprintCallable)
	void LockOntoTarget(AActor *LockOnTarget);

	UFUNCTION(BlueprintCallable)
	void SetFireMode(bool DoesFire);
};

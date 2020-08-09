// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "TankPawn.h"
#include "SimpleAITankPawn.generated.h"

/**
 * 
 */
UCLASS()
class PANZERSPIEL_API ASimpleAITankPawn : public ATankPawn
{
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
	
	UPROPERTY(EditDefaultsOnly, Category="AI")
	UArrowComponent *SensorLeft;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UArrowComponent *SensorHalfLeft;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UArrowComponent *SensorFront;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UArrowComponent *SensorHalfRight;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	UArrowComponent *SensorRight;

	UPROPERTY()
	bool FollowingPathPoints;

	UPROPERTY()
	class UBTTask_SimpleTankMoveTo *CallingTask;

	UPROPERTY(EditAnywhere, Category="AI")
	TArray<FVector> PathPoints;

	UPROPERTY()
	int32 CurrentPathPoint = 0;

	// The radius at which we reached the target.
	UPROPERTY(EditAnywhere, Category="AI")
	int32 ReachRadius = 5;

	// Functions
	virtual void MoveRight(float AxisValue) override;

public:
	ASimpleAITankPawn();

	UFUNCTION()
    void NavigationTrace();

	// Called every frame.
	virtual void Tick(float DeltaTime) override;

	bool FollowPathPoints(UBTTask_SimpleTankMoveTo *Task, TArray<FVector> Points);
};

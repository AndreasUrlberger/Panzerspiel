// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "TankCharacter.h"
#include "AITankCharacter.generated.h"

/**
 * 
 */
UCLASS()
class PANZERSPIEL_API AAITankCharacter : public ATankCharacter {
	GENERATED_BODY()

	// Variables
private:
	UPROPERTY(EditAnywhere, Category="AI")
	float TraceRange;

	UPROPERTY(VisibleAnywhere, Category="AI")
	TArray<float> Distances;

	// Sensors
	UPROPERTY(VisibleAnywhere, Category="AI")
	TArray<UArrowComponent*> Sensors;
	
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

	// Functions
	virtual void MoveRight(float AxisValue) override;

public:
	AAITankCharacter();
	
	UFUNCTION()
	bool MoveTo(FVector TargetLocation, float DeltaTime);

	UFUNCTION()
	void NavigationTrace();

	// Called every frame.
	virtual void Tick(float DeltaTime) override;
};

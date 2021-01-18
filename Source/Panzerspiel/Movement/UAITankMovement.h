// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"


#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/MovementComponent.h"

/**
 * 
 */
class PANZERSPIEL_API UAITankMovement : public UActorComponent {

	// Variables.
private:
	UPROPERTY()
	FVector FakeVelocity;
	UPROPERTY()
	bool bIsStopped;
	UPROPERTY(EditDefaultsOnly)
	float VELOCITY_DECREASE_SPEED = 1;

	UPROPERTY()
	UFloatingPawnMovement *ParentMoveComp;


	// Functions.
public:
	UAITankMovement();
	~UAITankMovement();

	UFUNCTION()
	void Init(UFloatingPawnMovement* MovementComponent);

	UFUNCTION()
	void Stop();

	UFUNCTION()
	void AddInputVector(FVector Input);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};

// All rights reserved @Apfelstrudel Games.


#include "UAITankMovement.h"

UAITankMovement::UAITankMovement() {
}

UAITankMovement::~UAITankMovement() {
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UAITankMovement::Init(UFloatingPawnMovement* MovementComponent) {
	ParentMoveComp = MovementComponent;
}

void UAITankMovement::Stop() {
	bIsStopped = true;
}

void UAITankMovement::AddInputVector(FVector Input) {
	ParentMoveComp->AddInputVector(Input);
	FakeVelocity = Input;
	FakeVelocity.Z = 0;
}

void UAITankMovement::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	if(bIsStopped)
		FakeVelocity = FMath::VInterpConstantTo(FakeVelocity, FVector::ZeroVector, DeltaTime, VELOCITY_DECREASE_SPEED);
}

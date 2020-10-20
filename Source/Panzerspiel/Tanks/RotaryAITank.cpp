// All rights reserved @Apfelstrudel Games.


#include "RotaryAITank.h"

#include "Containers/Array.h"
#include "../BTTask_SimpleTankMoveTo.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "../PanzerspielGameModeBase.h"

void ARotaryAITank::MoveRight(float AxisValue) {
    // Do nothing.
}

ARotaryAITank::ARotaryAITank() {
}

void ARotaryAITank::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    // Always update the TimeTillNextShot before calling ShootIfPossible.
    TimeTillNextShot -= DeltaTime;
    ShootIfPossible(DeltaTime);
}

bool ARotaryAITank::ShootIfPossible(float DeltaTime) {
    // TODO: Make the turret slowly move back to aiming at the target after firing to avoid a second jump.
    // Only shoot if we're in fire mode and we're ready to shoot.
    if (!(FireMode && TimeTillNextShot <= 0))
        return false;
    
    UWorld* World = GetWorld();
    if (!World)
        return false;

    // Update the turret rotation.
    const FRotator UpdatedRotation = TurretMesh->GetComponentRotation().Add(0, DeltaTime * TowerRotationSpeed, 0);
    TurretMesh->SetWorldRotation(UpdatedRotation);

    // Do single line trace.
    FHitResult Result;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    const FVector StartLoc = Muzzle->GetComponentLocation();
    const FVector EndLoc = StartLoc + Muzzle->GetForwardVector() * TraceDistance;
    World->LineTraceSingleByChannel(Result, StartLoc, EndLoc, COLLISION_BULLET_TRACE, Params);

    if(Cast<ATankPawn>(Result.GetActor())) {
        // Found valid path.
        AlignTower(EndLoc);
        if(!Shoot())
            return false;
        // Successfully fired bullet.
        // Renew TimeTillNextShot.
        TimeTillNextShot = FMath::RandRange(MinShootDelay, RandomShootDelay);
        return true;
    }else {
        return false;
    }
}

void ARotaryAITank::SetFireMode(bool DoesFire) {
    FireMode = DoesFire;
}

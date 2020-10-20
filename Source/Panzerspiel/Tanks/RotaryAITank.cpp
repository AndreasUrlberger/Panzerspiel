// All rights reserved @Apfelstrudel Games.


#include "RotaryAITank.h"


#include "DrawDebugHelpers.h"
#include "Containers/Array.h"
#include "../BTTask_SimpleTankMoveTo.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "../PanzerspielGameModeBase.h"
#include "Panzerspiel/Utility.h"

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

    FVector Target;
    bool hasLOS = CheckDirectLOS(Target);
    if(!hasLOS) hasLOS = CheckSingleRicochetLOS(Target);
    if(hasLOS) {
        // Found valid path.
        AlignTower(Target);
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

bool ARotaryAITank::CheckDirectLOS(FVector& OutTarget) {
    UWorld* World = GetWorld();
    if (!World)
        return false;
    // Do single line trace.
    FHitResult Result;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    const FVector StartLoc = Muzzle->GetComponentLocation();
    const FVector EndLoc = StartLoc + Muzzle->GetForwardVector() * TraceDistance;
    World->LineTraceSingleByChannel(Result, StartLoc, EndLoc, COLLISION_BULLET_TRACE, Params);
    OutTarget = EndLoc;
    return Cast<ATankPawn>(Result.GetActor()) != nullptr;
}

bool ARotaryAITank::CheckSingleRicochetLOS(FVector& OutTarget) {
    UWorld* World = GetWorld();
    if (!World)
        return false;
    
    // Do single line trace.
    FHitResult Result;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    FVector StartLoc = Muzzle->GetComponentLocation();
    FVector EndLoc = StartLoc + Muzzle->GetForwardVector() * TraceDistance;
    World->LineTraceSingleByChannel(Result, StartLoc, EndLoc, COLLISION_BULLET_TRACE, Params);
    OutTarget = EndLoc;

    // Second Trace.
    StartLoc = Result.ImpactPoint;
    const FVector2D ReflectNormal = UUtility::MirrorVector(FVector2D(Muzzle->GetForwardVector()), FVector2D(StartLoc), FVector2D(Result.ImpactNormal));
    EndLoc = StartLoc - FVector(ReflectNormal.X, ReflectNormal.Y, 0) * TraceDistance;
    Params.ClearIgnoredActors();
    Params.AddIgnoredActor(Result.GetActor());
    World->LineTraceSingleByChannel(Result, StartLoc, EndLoc, COLLISION_BULLET_TRACE, Params);
    DrawDebugLine(World, StartLoc, Result.ImpactPoint, FColor::Green, false, 1, 0, 5);

    return Cast<ATankPawn>(Result.GetActor()) && Result.GetActor() != this;
}

bool ARotaryAITank::CheckDoubleRicochetLOS(FVector& OutTarget) {
    UWorld* World = GetWorld();
    if (!World)
        return false;
    // Do single line trace.
    FHitResult Result;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);
    const FVector StartLoc = Muzzle->GetComponentLocation();
    const FVector EndLoc = StartLoc + Muzzle->GetForwardVector() * TraceDistance;
    World->LineTraceSingleByChannel(Result, StartLoc, EndLoc, COLLISION_BULLET_TRACE, Params);
    OutTarget = EndLoc;
    return Cast<ATankPawn>(Result.GetActor()) != nullptr;
}

void ARotaryAITank::SetFireMode(bool DoesFire) {
    FireMode = DoesFire;
}

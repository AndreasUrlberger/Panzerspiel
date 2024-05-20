// All rights reserved @Apfelstrudel Games.


#include "RotaryAITank.h"


#include "DrawDebugHelpers.h"
#include "Containers/Array.h"
#include "../BTTask_SimpleTankMoveTo.h"
#include "GameFramework/FloatingPawnMovement.h"
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
    CurrentRotTime += DeltaTime * TowerRotationSpeed;
    RotAndShoot();
}

bool ARotaryAITank::RotAndShoot() {
    
    // Update the turret rotation.
    const float QueriedRotValue = GetRotValue();
    const float RotValue = QueriedRotValue - LastRotValue;
    LastRotValue = QueriedRotValue;
    const FRotator UpdatedRotation = TurretMesh->GetComponentRotation().Add(0, RotValue, 0);
    TurretMesh->SetWorldRotation(UpdatedRotation);
    
    // Only shoot if we're in fire mode and we're ready to shoot.
    if (!(FireMode && TimeTillNextShot <= 0))
        return false;

    FVector Target;
    bool hasLOS = CheckDirectLOS(Target);
    if(!hasLOS) hasLOS = CheckSingleRicochetLOS(Target);
    if(!hasLOS) hasLOS = CheckDoubleRicochetLOS(Target);
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

bool ARotaryAITank::CheckDirectLOS(FVector& OutTarget) const{
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

bool ARotaryAITank::CheckSingleRicochetLOS(FVector& OutTarget) const{
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

    return Cast<ATankPawn>(Result.GetActor()) && Result.GetActor() != this;
}

bool ARotaryAITank::CheckDoubleRicochetLOS(FVector& OutTarget) const{
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
    FVector2D ReflectDir = UUtility::MirrorVector(FVector2D(Muzzle->GetForwardVector()), FVector2D(StartLoc), FVector2D(Result.ImpactNormal));
    EndLoc = StartLoc - FVector(ReflectDir.X, ReflectDir.Y, 0) * TraceDistance;
    Params.ClearIgnoredActors();
    Params.AddIgnoredActor(Result.GetActor());
    World->LineTraceSingleByChannel(Result, StartLoc, EndLoc, COLLISION_BULLET_TRACE, Params);

    // Second Trace.
    StartLoc = Result.ImpactPoint;
    ReflectDir = UUtility::MirrorVector(ReflectDir, FVector2D(StartLoc), FVector2D(Result.ImpactNormal));
    EndLoc = StartLoc + FVector(ReflectDir.X, ReflectDir.Y, 0) * TraceDistance;
    Params.ClearIgnoredActors();
    Params.AddIgnoredActor(Result.GetActor());
    World->LineTraceSingleByChannel(Result, StartLoc, EndLoc, COLLISION_BULLET_TRACE, Params);
    DrawDebugLine(World, StartLoc, Result.ImpactPoint, FColor::Cyan, false, 1, 0, 5);

    return Cast<ATankPawn>(Result.GetActor()) && Result.GetActor() != this;
}

float ARotaryAITank::GetRotValue(){
    while(CurrentRotTime >= 2)
        CurrentRotTime -= 2;
    // 2|x - 1| - 1.
    // Goes from (0|1) to (1|-1) to (2|1).
    return (2 * FMath::Abs(CurrentRotTime - 1) - 1) * AngleLimit;
}

void ARotaryAITank::SetFireMode(bool DoesFire) {
    FireMode = DoesFire;
}
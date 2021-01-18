// All rights reserved @Apfelstrudel Games.


#include "SimpleAITankPawn.h"

#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "Containers/Array.h"
#include "BTTask_SimpleTankMoveTo.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "PanzerspielGameModeBase.h"

void ASimpleAITankPawn::MoveRight(float AxisValue) {
    // Do nothing.
}

ASimpleAITankPawn::ASimpleAITankPawn() {
    // We fill the array at the beginning so we dont have to check if an index exists each tick.
    Distances.Init(INT32_MAX, 5);

    SensorsRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SensorsRootComponent"));
    SensorsRoot->SetupAttachment(BaseMesh);
    
    // Create Sensors.
    SensorLeft = CreateDefaultSubobject<UArrowComponent>(TEXT("SensorLeft"));
    SensorHalfLeft = CreateDefaultSubobject<UArrowComponent>(TEXT("SensorHalfLeft"));
    SensorFront = CreateDefaultSubobject<UArrowComponent>(TEXT("SensorFront"));
    SensorHalfRight = CreateDefaultSubobject<UArrowComponent>(TEXT("SensorHalfRight"));
    SensorRight = CreateDefaultSubobject<UArrowComponent>(TEXT("SensorRight"));
    SensorLeft->SetupAttachment(SensorsRoot);
    SensorHalfLeft->SetupAttachment(SensorsRoot);
    SensorFront->SetupAttachment(SensorsRoot);
    SensorHalfRight->SetupAttachment(SensorsRoot);
    SensorRight->SetupAttachment(SensorsRoot);
    // Fill them in the Sensors array.
    Sensors.Add(SensorLeft);
    Sensors.Add(SensorHalfLeft);
    Sensors.Add(SensorFront);
    Sensors.Add(SensorHalfRight);
    Sensors.Add(SensorRight);
}

void ASimpleAITankPawn::DoNavigationTrace() {
    if(UWorld *World = GetWorld()) {
        int8 Length = Sensors.Num();
        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);
        for(int8 Index = 0; Index < Length; ++Index) {
            // Do raycast.
            UArrowComponent *Sensor = Sensors[Index];
            FVector StartLoc = Sensor->GetComponentLocation();
            FVector EndLoc = StartLoc + Sensor->GetForwardVector() * TraceRange;
            World->LineTraceSingleByChannel(HitResult, StartLoc, EndLoc, ECollisionChannel::ECC_Camera, Params);
            // Store gained data.
            if(HitResult.bBlockingHit)
                Distances[Index] = HitResult.Distance;
            else
                Distances[Index] = INT32_MAX;

            // Debug.
            DrawDebugLine(World, StartLoc, EndLoc, FColor::Red);
        }
    }
}

void ASimpleAITankPawn::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    DoNavigationTrace();   
    if(FollowingPathPoints)
        FollowPath(DeltaTime);
    else
        //FakeVelocity = FMath::VInterpConstantTo()
    // Lock on aim.
    if(IsValid(LockOnActor))
        AlignTower(LockOnActor->GetActorLocation());
    // Always update the TimeTillNextShot before calling ShootIfPossible.
    TimeTillNextShot -= DeltaTime;
    ShootIfPossible();
}

void ASimpleAITankPawn::FollowPath(float DeltaTime) {
    const float Distance = FVector::Dist(PathPoints[CurrentPathPoint], GetActorLocation());
    if(DebugLog) UE_LOG(LogTemp, Warning, TEXT("Called FollowPath"));
    if (Distance < ReachRadius) {
        if (DebugLog) UE_LOG(LogTemp, Warning, TEXT("Reached Point"));
        ++CurrentPathPoint;
        if (CurrentPathPoint >= PathPoints.Num()) {
            // Reached end of path.
            FollowingPathPoints = false;
            CallingTask->Finish();
            // Stop the tank's movement.
            //MovementComp->
        }
    } else {
        // Calculate in which direction to move to avoid collisions.
        // TODO: Each Sensor should have its own AvoidDistance and maybe some weight.
        const FVector AvoidVector = GetAvoidVector();

        // Follow Path.
        const FVector Target = PathPoints[CurrentPathPoint];
        const FVector Desired = (Target - GetActorLocation()).GetUnsafeNormal();
        FVector DeltaMove = ((Desired + AvoidVector) / 2 + FakeVelocity * VelocityImpact).GetUnsafeNormal();
        //UE_LOG(LogTemp, Warning, TEXT("DeltaMove before: %s"), *DeltaMove.ToString());
        // Updates the DeltaMove to follow the tanks movement rules.
        DeltaMove = UpdateMovement(DeltaTime, DeltaMove);
        //UE_LOG(LogTemp, Warning, TEXT("DeltaMove after: %s"), *DeltaMove.ToString());

        // We dont need to calculate DeltaMove.Size() since its always 1 as it gets normalized just a few lines above.
        const float DeltaSize = MovementComp->MaxSpeed * DeltaTime;
        // Dont move farther than the target.
        if (DeltaSize > Distance)
            DeltaMove *= Distance / DeltaSize;

        //UE_LOG(LogTemp, Warning, TEXT("Desired: %s, DeltaMove: %s, FakeVelocity: %s"), *Desired.ToString(), *DeltaMove.ToString(), *FakeVelocity.ToString())
        MovementComp->AddInputVector(DeltaMove);

        FakeVelocity = DeltaMove;
        FakeVelocity.Z = 0;

        SetActorRotation((DeltaMove * Direction).Rotation());

    }
}

bool ASimpleAITankPawn::ShootIfPossible() {
    // TODO: Make the turret slowly move back to aiming at the target after firing to avoid a second jump.
    // Only shoot if a target is selected, we're in fire mode and we're ready to shoot.
    if (!(IsValid(LockOnActor) && FireMode && TimeTillNextShot <= 0))
        return false;
    
    UWorld* World = GetWorld();
    if (!World)
        return false;

    FVector TargetLocation;
    const FVector FiringLocation = TurretMesh->GetComponentLocation();
    if(!GameMode->GetDirectPath(this, FiringLocation, LockOnActor, TargetLocation)) {
        if(!GameMode->GetShortestSingleRicochet(this, FiringLocation, LockOnActor, TargetLocation)) {
            if(!GameMode->GetShortestDoubleRicochet(this, FiringLocation, LockOnActor, TargetLocation)) {
                if(DebugLog) UE_LOG(LogTemp, Warning, TEXT("Could not find any Path"));
                return false;
            }else {
                if(DebugLog) UE_LOG(LogTemp, Warning, TEXT("Found double ricochet Path"));
            }
        }else {
            if(DebugLog) UE_LOG(LogTemp, Warning, TEXT("Found single ricochet Path"));
        }
    }else {
        if(DebugLog) UE_LOG(LogTemp, Warning, TEXT("Found direct Path"));
    }
                

    // Found valid path.
    AlignTower(TargetLocation);
    if(!Shoot())
        return false;
    // Successfully fired bullet.
    // Renew TimeTillNextShot.
    TimeTillNextShot = FMath::RandRange(MinShootDelay, RandomShootDelay);
    return true;
}

bool ASimpleAITankPawn::FollowPathPoints(UBTTask_SimpleTankMoveTo *Task, TArray<FVector> Points) {
    CallingTask = Task;
    PathPoints = Points;
    CurrentPathPoint = 0;
    // NavSystem always gives a value in the air.
    for (int32 Index = 0; Index < PathPoints.Num(); ++Index)
        PathPoints[Index].Z = 0;
    FollowingPathPoints = true;
    return true;
}

void ASimpleAITankPawn::LockOntoTarget(AActor* LockOnTarget) {
    LockOnActor = LockOnTarget;
}

void ASimpleAITankPawn::SetFireMode(bool DoesFire) {
    FireMode = DoesFire;
}

FVector ASimpleAITankPawn::GetAvoidVector() {
    // Following loop would break if we dont have at least as many AvoidDistances as we have sensors.
    check(AvoidDistances.Num() >= Distances.Num());
    FVector AvoidVector = FVector::ZeroVector;
    for(int32 Index = 0; Index < Distances.Num(); ++Index) {
        AvoidVector -= Sensors[Index]->GetForwardVector() * FMath::Square(AvoidDistances[Index]/Distances[Index]);
    }
    // Get the middle of the current and last avoidVector to achieve a smoothing effect.
    if(bSmoothAvoidVector) {
        AvoidVector = (AvoidVector + LastAvoidVector) / 2;
        LastAvoidVector = AvoidVector;
    }
    AvoidVector = (AvoidVector * AvoidStrength);
    //UE_LOG(LogTemp, Warning, TEXT("Avoidance Vector: %s"), *AvoidVector.ToString());
    return AvoidVector;
}

FVector ASimpleAITankPawn::UpdateMovement(float DeltaTime, const FVector DesiredDeltaMove) {
    // TODO: Tanks need to slow down when they want to drive a tight corner.
    // TODO: Current point on path should be determined by how close it is to a point (right angle distance).
    FVector Forward = GetActorForwardVector() * Direction;
    FVector DesiredDirection = DesiredDeltaMove;
    // Better safe than sorry.
    Forward.Z = 0;
    DesiredDirection.Z = 0;

    // Positive if smaller than 90 deg and Negative if bigger.
    if(FVector::DotProduct(Forward, DesiredDirection) < 0) {
        // Target is in opposite direction thus we change direction.
        Direction *= -1;
        Forward *= -1;
        // Orient the Sensor in the other direction.
        SensorsRoot->AddRelativeRotation(FRotator(0, 180, 0));
    }
    const FVector NewForward = FMath::VInterpNormalRotationTo(Forward, DesiredDirection, DeltaTime, RotationSpeed);
    //if(DebugLog) UE_LOG(LogTemp, Warning, TEXT("New Forward Vector: %s"), *NewForward.ToString());
    SetActorRotation(NewForward.Rotation());

    FVector CurrentLoc = GetActorLocation();
    FVector TargetLoc = CurrentLoc + DesiredDeltaMove;
    // Same as above.
    CurrentLoc.Z = 0;
    TargetLoc.Z = 0;
    const FVector NewDeltaMove = NewForward * DeltaTime * MovementSpeed;
    
    return NewDeltaMove;
}

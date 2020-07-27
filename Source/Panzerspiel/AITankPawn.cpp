// All rights reserved @Apfelstrudel Games.


#include "AITankPawn.h"
#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "Containers/Array.h"

void AAITankPawn::MoveRight(float AxisValue) {
    // Do nothing.
}

AAITankPawn::AAITankPawn() {
    // We fill the array at the beginning so we dont have to check if an index exists each tick.
    Distances.Init(0, 5);

    // Create Sensors.
    SensorLeft = CreateDefaultSubobject<UArrowComponent>(TEXT("SensorLeft"));
    SensorHalfLeft = CreateDefaultSubobject<UArrowComponent>("SensorHalfLeft");
    SensorFront = CreateDefaultSubobject<UArrowComponent>("SensorFront");
    SensorHalfRight = CreateDefaultSubobject<UArrowComponent>("SensorHalfRight");
    SensorRight = CreateDefaultSubobject<UArrowComponent>("SensorRight");
    SensorLeft->SetupAttachment(BaseMesh);
    SensorHalfLeft->SetupAttachment(BaseMesh);
    SensorFront->SetupAttachment(BaseMesh);
    SensorHalfRight->SetupAttachment(BaseMesh);
    SensorRight->SetupAttachment(BaseMesh);
    // Fill them in the Sensors array.
    Sensors.Add(SensorLeft);
    Sensors.Add(SensorHalfLeft);
    Sensors.Add(SensorFront);
    Sensors.Add(SensorHalfRight);
    Sensors.Add(SensorRight);
}

bool AAITankPawn::MoveTo(FVector TargetLocation, float DeltaTime) {
    UE_LOG(LogTemp, Warning, TEXT("MoveTo: TargetLocation: %s"), *TargetLocation.ToString());
    bool bReachedTarget = false;
    const FVector CurrentLocation = GetActorLocation();
    FVector Route = TargetLocation - CurrentLocation;
    // For some reason the Path tells the object to move up.
    Route.Z = 0;
    FVector Direction;
    float Length;
    Route.ToDirectionAndLength(Direction, Length);

    // Check if tank looks towards TargetDestination
    // If Yes -> move there
    // If no ->
    // Calculate degree the tank has to rotate
    // Calculate what value to pass to MoveRightAxisValue

    FVector DeltaMove = Direction * MovementSpeed * DeltaTime;
    if(DeltaMove.SizeSquared2D() - Route.SizeSquared2D() >= 0) {
        // Tank would move too far -> we reached the TargetLocation.
        DeltaMove = Route;
        UE_LOG(LogTemp, Warning, TEXT("Reached Point"));
        bReachedTarget = true;
    }
    SetActorLocation(CurrentLocation + DeltaMove, false);
    SetActorRotation(Direction.Rotation());

    // Tells whether we reached the TargetLocation.
    return bReachedTarget;
}

void AAITankPawn::NavigationTrace() {
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
                Distances[Index] = -1;

            // Debug.
            DrawDebugLine(World, StartLoc, EndLoc, FColor::Red);
        }
    }
}

void AAITankPawn::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    NavigationTrace();
}
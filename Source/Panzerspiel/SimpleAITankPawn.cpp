// All rights reserved @Apfelstrudel Games.


#include "SimpleAITankPawn.h"
#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "Containers/Array.h"
#include "BTTask_SimpleTankMoveTo.h"
#include "GameFramework/FloatingPawnMovement.h"

void ASimpleAITankPawn::MoveRight(float AxisValue) {
    // Do nothing.
}

ASimpleAITankPawn::ASimpleAITankPawn() {
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

void ASimpleAITankPawn::NavigationTrace() {
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

void ASimpleAITankPawn::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    if(FollowingPathPoints) {
        const float Distance = FVector::Dist(PathPoints[CurrentPathPoint], GetActorLocation());
        if(Distance < ReachRadius) {
            UE_LOG(LogTemp, Warning, TEXT("Reached Point"));
            ++CurrentPathPoint;
            if(CurrentPathPoint >= PathPoints.Num()) {
                // Reached end of path.
                FollowingPathPoints = false;
                CallingTask->Abort();    
            }
        }else {
            // Follow Path.
            const FVector Target = PathPoints[CurrentPathPoint];
            const FVector Desired =  Target - GetActorLocation();
            FVector CurrentVelocity = MovementComp->Velocity;
            CurrentVelocity.Z = 0;
            FVector DeltaMove = Desired;// - CurrentVelocity;
            DeltaMove = DeltaMove.GetUnsafeNormal();
            UE_LOG(LogTemp, Warning, TEXT("CurrentPosition: %s, DeltaMove: %s, CurrentVelocity: %s"), *GetActorLocation().ToCompactString(), *DeltaMove.ToCompactString(), *CurrentVelocity.ToCompactString())
            MovementComp->AddInputVector(DeltaMove);
        }
    }
    NavigationTrace();
}

bool ASimpleAITankPawn::FollowPathPoints(UBTTask_SimpleTankMoveTo *Task, TArray<FVector> Points) {
    CallingTask = Task;
    PathPoints = Points;
    // NavSystem always gives a value in the air.
    for (int32 Index = 0; Index < PathPoints.Num(); ++Index)
        PathPoints[Index].Z = 0;
    FollowingPathPoints = true;
    return true;
}

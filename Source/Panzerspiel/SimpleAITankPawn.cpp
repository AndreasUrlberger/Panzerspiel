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
    Distances.Init(INT32_MAX, 5);

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
                Distances[Index] = INT32_MAX;

            // Debug.
            DrawDebugLine(World, StartLoc, EndLoc, FColor::Red);
        }
    }
}

void ASimpleAITankPawn::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    NavigationTrace();
    
    if(FollowingPathPoints) {
        const float Distance = FVector::Dist(PathPoints[CurrentPathPoint], GetActorLocation());
        if(Distance < ReachRadius) {
            UE_LOG(LogTemp, Warning, TEXT("Reached Point"));
            ++CurrentPathPoint;
            if(CurrentPathPoint >= PathPoints.Num()) {
                // Reached end of path.
                FollowingPathPoints = false;
                CallingTask->Finish();    
            }
        }else {
            // Calculate in which direction to move to avoid collisions.
            FVector AvoidVector = FVector::ZeroVector;
            if(Distances[4] < AvoidDistance) {
                AvoidVector += -GetActorRightVector() * (AvoidDistance/Distance);
            }
            AvoidVector = (AvoidVector * AvoidStrength);
            UE_LOG(LogTemp, Warning, TEXT("Avoidance Vector: %s"), *AvoidVector.ToString());

            // Follow Path.
            const FVector Target = PathPoints[CurrentPathPoint];
            const FVector Desired =  (Target - GetActorLocation()).GetUnsafeNormal();
            FVector DeltaMove = ((Desired + AvoidVector)/2 + FakeVelocity * VelocityImpact).GetUnsafeNormal();
            FakeVelocity = DeltaMove;
            FakeVelocity.Z = 0;

            // We dont need to calculate DeltaMove.Size() since its always 1 as it gets normalized just a few lines above.
            const float DeltaSize = MovementComp->MaxSpeed * DeltaTime;
            // Dont move farther than the target.
            if(DeltaSize > Distance)
                DeltaMove *= Distance / DeltaSize;
            
            UE_LOG(LogTemp, Warning, TEXT("Desired: %s, DeltaMove: %s, FakeVelocity: %s"), *Desired.ToString(), *DeltaMove.ToString(), *FakeVelocity.ToString())
            MovementComp->AddInputVector(DeltaMove);
        }
    }
    NavigationTrace();
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

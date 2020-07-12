// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPlayerController.h"

ATankPlayerController::ATankPlayerController()
{
    
}

void ATankPlayerController::Tick(float DeltaSeconds)
{
    if (Crosshair)
    {
        FHitResult Result = FHitResult(20);
        GetHitResultUnderCursor(ECollisionChannel::ECC_EngineTraceChannel2, false, Result);

        Crosshair->SetActorLocation(Result.Location + FVector(0, 0, CrosshairHeight));

        UE_LOG(LogTemp, Warning, TEXT("Rsult location: %s: "), *Result.Location.ToCompactString());
    }
}

void ATankPlayerController::BeginPlay()
{
    if (CrosshairToSpawn)
    {
        if (UWorld* World = GetWorld())
        {
            FActorSpawnParameters Params;
            Params.Owner = this;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            FRotator Rotation = FRotator(0, 0, 0);
            FVector Location = FVector(0, 0, 0);

            Crosshair = World->SpawnActor<AActor>(CrosshairToSpawn, Location, Rotation, Params);
        }
    }
}

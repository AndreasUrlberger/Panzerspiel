// Fill out your copyright notice in the Description page of Project Settings.
#define CrosshairPlaneTraceChannel ETraceTypeQuery::TraceTypeQuery3


#include "TankPlayerController.h"
#include "TankPawn.h"

ATankPlayerController::ATankPlayerController() {
}

void ATankPlayerController::Tick(float DeltaSeconds) {    
    // Move Crosshair to cursor location if Crosshair was created.
    if (Crosshair) {
        FHitResult Result = FHitResult(20);
        GetHitResultUnderCursorByChannel(CrosshairPlaneTraceChannel, false, Result);

        Crosshair->SetActorLocation(Result.Location + FVector(0, 0, CrosshairHeight));

        // If controlled Pawn is a TankPawn we align its tower to point towards the crosshair.
        if (ATankPawn* TankPawn = Cast<ATankPawn>(GetPawn())) {
            TankPawn->AlignTower(Result.Location);
        }
    }
}

void ATankPlayerController::BeginPlay() {
    // Spawn Crosshair if blueprint is given.
    if (CrosshairToSpawn) {
        if (UWorld* World = GetWorld()) {
            FActorSpawnParameters Params;
            Params.Owner = this;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            FRotator Rotation = FRotator(0, 0, 0);
            FVector Location = FVector(0, 0, 0);

            Crosshair = World->SpawnActor<AActor>(CrosshairToSpawn, Location, Rotation, Params);
        }
    }
}

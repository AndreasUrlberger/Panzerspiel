// Copyright Apfelstrudel Games - All Rights Reserved
#define CrosshairPlaneTraceChannel ETraceTypeQuery::TraceTypeQuery3


#include "TankPlayerController.h"
#include "TankPawn.h"
#include "../Plugins/Marketplace/UINavigation/Source/UINavigation/Public/UINavPCComponent.h"

ATankPlayerController::ATankPlayerController() {
}

void ATankPlayerController::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    if(ControllerInput) {
        ControllerUpdateCrosshair(DeltaTime);
    }else {
        UpdateCrosshair();
    }
    
}

void ATankPlayerController::BeginPlay() {
    Super::BeginPlay();
    // Spawn Crosshair if blueprint is given.
    if (CrosshairToSpawn) {
        if (UWorld* World = GetWorld()) {
            FActorSpawnParameters Params;
            Params.Owner = this;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            const FRotator Rotation = FRotator(0, 0, 0);
            const FVector Location = FVector(0, 0, 0);

            Crosshair = World->SpawnActor<AActor>(CrosshairToSpawn, Location, Rotation, Params);
        }
    }
}

void ATankPlayerController::SetCrosshairVisibility(bool IsVisible) {
    Crosshair->SetActorHiddenInGame(!IsVisible);
}

void ATankPlayerController::SetupInputComponent() {
    Super::SetupInputComponent();
    if(ControllerInput) {
        InputComponent->BindAxis("MoveCrosshairUp", this, &ATankPlayerController::CrosshairMoveUp);
        InputComponent->BindAxis("MoveCrosshairRight", this, &ATankPlayerController::CrosshairMoveRight);
        InputComponent->BindAxis("ControllerMoveForward", this, &ATankPlayerController::ControllerMoveForward);
        InputComponent->BindAxis("ControllerMoveRight", this, &ATankPlayerController::ControllerMoveRight);
    } else {
        InputComponent->BindAxis("MoveForward", this, &ATankPlayerController::MoveForward);
        InputComponent->BindAxis("MoveRight", this, &ATankPlayerController::MoveRight);
    }

    InputComponent->BindAction("Shoot", EInputEvent::IE_Pressed, this, &ATankPlayerController::FireButtonPressed);
    InputComponent->BindAction("PlaceMine", EInputEvent::IE_Pressed, this, &ATankPlayerController::MineButtonPressed);
}

void ATankPlayerController::FireButtonPressed() {
    ATankPawn *TankPawn = Cast<ATankPawn>(GetPawn());
    if(IsValid(TankPawn))
        TankPawn->Shoot();   
}

void ATankPlayerController::MineButtonPressed() {
    ATankPawn *TankPawn = Cast<ATankPawn>(GetPawn());
    if(IsValid(TankPawn))
        TankPawn->PlaceMine();
}

void ATankPlayerController::MoveForward(float AxisValue) {
    ATankPawn *TankPawn = Cast<ATankPawn>(GetPawn());
    if(IsValid(TankPawn))
        TankPawn->MoveForward(AxisValue);
}

void ATankPlayerController::MoveRight(float AxisValue) {
    ATankPawn *TankPawn = Cast<ATankPawn>(GetPawn());
    if(IsValid(TankPawn))
        TankPawn->MoveRight(AxisValue);
}

void ATankPlayerController::ControllerMoveForward(float AxisValue) {
    ATankPawn *TankPawn = Cast<ATankPawn>(GetPawn());
    if(IsValid(TankPawn))
        TankPawn->ControllerMoveForward(AxisValue);
}

void ATankPlayerController::ControllerMoveRight(float AxisValue) {
    ATankPawn *TankPawn = Cast<ATankPawn>(GetPawn());
    if(IsValid(TankPawn))
        TankPawn->ControllerMoveRight(AxisValue);
}


void ATankPlayerController::UpdateCrosshair() const {
    // Move crosshair to cursor location if crosshair was created.
    if (IsValid(Crosshair)) {
        FHitResult Result = FHitResult(20);
        GetHitResultUnderCursorByChannel(CrosshairPlaneTraceChannel, false, Result);

        Crosshair->SetActorLocation(Result.Location + FVector(0, 0, CrosshairHeight));

        // If controlled Pawn is a TankPawn we align its tower to point towards the crosshair.
        if (ATankPawn* TankPawn = Cast<ATankPawn>(GetPawn())) {
            TankPawn->AlignTower(Result.Location);
        }
    }
}

void ATankPlayerController::ControllerUpdateCrosshair(float DeltaTime) const {
    if(IsValid(Crosshair)) {
        const FVector CurrentLocation = Crosshair->GetActorLocation();
        const FVector DeltaLocation = FVector(CrosshairUpValue, CrosshairRightValue, 0) * CrosshairMovementSpeed * DeltaTime;
        FVector NewLocation = CurrentLocation + DeltaLocation;
        NewLocation.X = FMath::Clamp(NewLocation.X, BorderBottom, BorderTop);
        NewLocation.Y = FMath::Clamp(NewLocation.Y, BorderLeft, BorderRight);
        Crosshair->SetActorLocation(NewLocation);

        // If controlled Pawn is a TankPawn we align its tower to point towards the crosshair.
        if (ATankPawn* TankPawn = Cast<ATankPawn>(GetPawn())) {
            TankPawn->AlignTower(CurrentLocation);
        }
    }
}

void ATankPlayerController::CrosshairMoveUp(float AxisValue) {
    CrosshairUpValue = AxisValue;
}

void ATankPlayerController::CrosshairMoveRight(float AxisValue) {
    CrosshairRightValue = AxisValue;
}

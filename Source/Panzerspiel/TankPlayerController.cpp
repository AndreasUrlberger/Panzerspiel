// Copyright Apfelstrudel Games - All Rights Reserved
#define CrosshairPlaneTraceChannel ETraceTypeQuery::TraceTypeQuery3


#include "TankPlayerController.h"
#include "TankPawn.h"
#include "../Plugins/Marketplace/UINavigation/Source/UINavigation/Public/UINavPCComponent.h"

ATankPlayerController::ATankPlayerController() {
}

void ATankPlayerController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (ControllerInput) {
		ControllerUpdateCrosshair(DeltaTime);
	} else {
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

void ATankPlayerController::OnPossess(APawn* InPawn) {
	Super::OnPossess(InPawn);
	// Make sure the possessed pawn uses the same input mode as specified in the controller.
	TankPawn = Cast<ATankPawn>(InPawn);
	if (IsValid(TankPawn))
		TankPawn->UseControllerMovement(ControllerInput);
}

void ATankPlayerController::OnUnPossess() {
	Super::OnUnPossess();
	TankPawn = nullptr;
}

void ATankPlayerController::UseControllerInput(bool UsesController) {
	ControllerInput = UsesController;
	if (IsValid(TankPawn))
		TankPawn->UseControllerMovement(ControllerInput);
}


void ATankPlayerController::SetCrosshairVisibility(bool IsVisible) {
	Crosshair->SetActorHiddenInGame(!IsVisible);
}

void ATankPlayerController::SetupInputComponent() {
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveCrosshairUp", this, &ATankPlayerController::CrosshairMoveUp);
	InputComponent->BindAxis("MoveCrosshairRight", this, &ATankPlayerController::CrosshairMoveRight);

	InputComponent->BindAxis("MoveForward", this, &ATankPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ATankPlayerController::MoveRight);

	InputComponent->BindAction("Shoot", EInputEvent::IE_Pressed, this, &ATankPlayerController::FireButtonPressed);
	InputComponent->BindAction("PlaceMine", EInputEvent::IE_Pressed, this, &ATankPlayerController::MineButtonPressed);
}

void ATankPlayerController::FireButtonPressed() {
	if (IsValid(TankPawn))
		TankPawn->Shoot();
}

void ATankPlayerController::MineButtonPressed() {
	if (IsValid(TankPawn))
		TankPawn->PlaceMine();
}

void ATankPlayerController::MoveForward(float AxisValue) {
	if (IsValid(TankPawn))
		TankPawn->MoveForward(AxisValue);
}

void ATankPlayerController::MoveRight(float AxisValue) {
	if (IsValid(TankPawn))
		TankPawn->MoveRight(FMath::Clamp(AxisValue, -1.0f, 1.0f));
}


void ATankPlayerController::UpdateCrosshair() const {
	// Move crosshair to cursor location if crosshair was created.
	if (IsValid(Crosshair)) {
		FHitResult Result = FHitResult(20);
		GetHitResultUnderCursorByChannel(CrosshairPlaneTraceChannel, false, Result);

		Crosshair->SetActorLocation(Result.Location + FVector(0, 0, CrosshairHeight));

		// If TankPawn is valid we align its tower to point towards the crosshair.
		if (IsValid(TankPawn))
			TankPawn->AlignTower(Result.Location);
	}
}

void ATankPlayerController::ControllerUpdateCrosshair(float DeltaTime) const {
	if (IsValid(Crosshair)) {
		const FVector CurrentLocation = Crosshair->GetActorLocation();
		const FVector DeltaLocation = FVector(CrosshairUpValue, CrosshairRightValue, 0) * CrosshairMovementSpeed *
			DeltaTime;
		FVector NewLocation = CurrentLocation + DeltaLocation;
		NewLocation.X = FMath::Clamp(NewLocation.X, BorderBottom, BorderTop);
		NewLocation.Y = FMath::Clamp(NewLocation.Y, BorderLeft, BorderRight);
		Crosshair->SetActorLocation(NewLocation);

		// If TankPawn is valid we align its tower to point towards the crosshair.
		if (IsValid(TankPawn))
			TankPawn->AlignTower(CurrentLocation);
	}
}

void ATankPlayerController::CrosshairMoveUp(float AxisValue) {
	CrosshairUpValue = AxisValue;
}

void ATankPlayerController::CrosshairMoveRight(float AxisValue) {
	CrosshairRightValue = AxisValue;
}

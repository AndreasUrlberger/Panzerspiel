// All rights reserved @Apfelstrudel Games.


#include "TankPawn.h"

#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Bullet.h"
#include "Kismet/GameplayStatics.h"
#include "Mine.h"
#include "PanzerspielGameModeBase.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Math/UnrealMathUtility.h"

// -------------------- Lifecycle -------------------- //

ATankPawn::ATankPawn() {
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	BaseMesh->SetupAttachment(RootComponent);
	SetRootComponent(BaseMesh);
	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>("TurretMesh");
	TurretMesh->SetupAttachment(BaseMesh);
	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>("MovementComponent");
}

// Called when the game starts or when spawned
void ATankPawn::BeginPlay() {
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
		GameMode = Cast<APanzerspielGameModeBase>(World->GetAuthGameMode());
	checkf(GameMode, TEXT("Could not get the GameMode. Note that GetAuthGameMode returns null on the client."));
	GameMode->TankPawnRegisterSelf(this);
}

// Called every frame
void ATankPawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	//if (DebugLog) UE_LOG(LogTemp, Warning, TEXT("Forward: %f, Right: %f"), MoveForwardAxisValue, MoveRightAxisValue);
	if (ControllerInput) {
		ControllerMove(DeltaTime);
	} else {
		const FVector DeltaLocation = GetActorForwardVector() * MoveForwardAxisValue * MovementSpeed * DeltaTime;
		MoveAndRotate(DeltaLocation, FRotator(0, MoveRightAxisValue * RotationSpeed * DeltaTime, 0));
	}
}

void ATankPawn::Kill(ATankPawn* Enemy) {
	Die();
}

void ATankPawn::Die() {
	if (ExplosionSound)
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	PlayNiagaraExplosion(GetActorLocation());
	GameMode->TankPawnRemoveSelf(this);
	Destroy();
}

// -------------------- Movement -------------------- //

void ATankPawn::MoveForward(float AxisValue) {
	MoveForwardAxisValue = AxisValue;
}

void ATankPawn::MoveRight(float AxisValue) {
	MoveRightAxisValue = AxisValue;
}

void ATankPawn::MoveAndRotate(const FVector DeltaLocation, const FRotator DeltaRotation) {
	SetActorRotation(GetActorRotation() + DeltaRotation);
	MovementComp->AddInputVector(DeltaLocation.GetSafeNormal());
}

void ATankPawn::CalculateActualMovement(FVector TargetLocation, float DeltaTime) {
	const FRotator CurrentRotation = GetActorRotation();
	const FRotator TargetDirection = (TargetLocation - GetActorLocation()).Rotation();
	FRotator DeltaRotator = TargetDirection - CurrentRotation;
	DeltaRotator.Normalize();
	const float DeltaYaw = DeltaRotator.Yaw;
	if (DebugLog) UE_LOG(LogTemp, Warning, TEXT("Yaw: %f"), DeltaYaw);

	const float MaxYaw = RotationSpeed * DeltaTime;
	if (FMath::Abs(DeltaYaw) <= MaxYaw) {
		// Rotate and move.
		const FVector DeltaLocation = TargetLocation - GetActorLocation();
		MoveAndRotate(DeltaLocation, DeltaRotator);
	} else {
		// Rotate and not move.
		const FRotator DeltaRotation = FRotator(0, FMath::Sign(DeltaYaw) * MaxYaw, 0);
		MoveAndRotate(FVector::ZeroVector, DeltaRotation);
	}
}

void ATankPawn::ControllerMove(float DeltaTime) {
	// TODO: Might need a deadzone threshold here.
	if (FMath::Abs(MoveForwardAxisValue) == 0 && FMath::Abs(MoveRightAxisValue) == 0) {
		// Theres no input so we dont want to move and especially dont want to change the tanks rotation.
		return;
	}

	FVector Forward = GetActorForwardVector() * Direction;
	const FVector DeltaMove = FVector(MoveForwardAxisValue, MoveRightAxisValue, 0);
	const float Size = DeltaMove.Size();

	// Better safe than sorry.
	Forward.Z = 0;

	// Positive if smaller than 90 deg and Negative if bigger.
	if (FVector::DotProduct(Forward, DeltaMove) < 0) {
		// Target is in opposite direction thus we change direction.
		Direction *= -1;
		Forward *= -1;
	}
	FVector NewForward = FMath::VInterpNormalRotationTo(Forward, DeltaMove, DeltaTime, RotationSpeed);
	// Make sure we keep the players input strength.
	NewForward = NewForward.GetUnsafeNormal() * Size;
	if (DebugLog) UE_LOG(LogTemp, Warning, TEXT("New Forward Vector: %s"), *NewForward.ToString());
	// Makes sure we arent suddenly switching backwards since Direction will neutralize itself.
	SetActorRotation((Direction * NewForward).Rotation());
	// Only move if were pointing in the target direction or if we are using the alternative controller movement.
	if (NewForward.Equals(DeltaMove) || AlternativeControllerMovement) {
		// Move
		MovementComp->AddInputVector(NewForward);
	}
}

void ATankPawn::UseControllerMovement(bool UseController) {
	ControllerInput = UseController;
	AlternativeControllerMovement = UseController;
}

// -------------------- Combat -------------------- //

void ATankPawn::AlignTower(const FVector Target) {
	FVector Rotation = Target - TurretMesh->GetComponentLocation();
	Rotation.Z = 0;

	TurretMesh->SetWorldRotation(Rotation.ToOrientationRotator(), true);
}

bool ATankPawn::Shoot() {
	if(ActiveShots >= MaxShots)
		return false;

	UWorld *World = GetWorld();
	if(World == nullptr)
		return false;
	
	// Spawn bullet.
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;
	const FRotator Rotation = TurretMesh->GetComponentRotation();
	const FVector Location = GetBulletSpawnPoint();
	ABullet* Bullet = World->SpawnActor<ABullet>(ToSpawnBullet, Location, Rotation, Params);


	// Check if the spawn was successful.
	if (Bullet) {
		++ActiveShots;
		Bullet->Init(this);
		if (FireSound)
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, Location);
	} else {
		// Player tried to shoot directly into a wall -> He dies.
		if (FireSound)
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, Location);
		Die();
	}

	// Bullet was successfully shot.
	return true;
}

void ATankPawn::PlaceMine() {
	if (ActiveMines >= MaxMines)
		return;

	if (UWorld* World = GetWorld()) {
		FActorSpawnParameters Params;
		Params.Owner = this;

		// Some kind of random rotation seems to be a good idea.
		const FRotator Rotation = TurretMesh->GetComponentRotation();
		const FVector Location = GetActorLocation();

		AMine* Mine = World->SpawnActor<AMine>(ToSpawnMine, Location, Rotation, Params);


		// Check if the spawn was successful.
		if (Mine) {
			++ActiveMines;
			Mine->Init(this);
			if (MinePlantSound)
				UGameplayStatics::PlaySoundAtLocation(this, MinePlantSound, Location);
		}
	}

}

FVector ATankPawn::GetBulletSpawnPoint() const {
	return TurretMesh->GetComponentLocation() + FVector(0, 0, BarrelHeight) + TurretMesh->GetComponentRotation().
		Vector() * BarrelLength;
}

void ATankPawn::TriggerBreakpoint() {
	FVector Z = FVector(9, 3, 4);
	Z *= 25;
}

void ATankPawn::BulletDestroyed() {
	check(ActiveShots > 0);
	--ActiveShots;
}

void ATankPawn::MineDestroyed() {
	check(ActiveMines > 0);
	--ActiveMines;
}

// -------------------- Other -------------------- //

int32 ATankPawn::GetTeam() const {
	return Team;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawn.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseBulletActor.h"

// Sets default values
ATankPawn::ATankPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>("PlayerMesh");
	RootComponent = PlayerMesh;
}

void ATankPawn::Shoot()
{
	if (ActiveShots < MaxShots) {
		if (UWorld* World = GetWorld()) {
			FActorSpawnParameters params;
			params.Owner = this;

			FRotator Rotation = GetActorRotation();
			FVector Location = GetBulletSpawnPoint();
			ABaseBulletActor* Bullet = World->SpawnActor<ABaseBulletActor>(ToSpawnBullet, Location, Rotation, params);
			Bullet->Init(this);
			++ActiveShots;
		}
	}
}

void ATankPawn::MoveForward(float AxisValue)
{
	MoveForwardAxisValue = AxisValue;
}

void ATankPawn::MoveRight(float AxisValue)
{
	MoveRightAxisValue = AxisValue;
}

FVector ATankPawn::GetBulletSpawnPoint()
{
	return GetActorLocation() + GetActorRotation().Vector() * BarrelLength;
}

void ATankPawn::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Would normaly die now"));
}

// Called when the game starts or when spawned
void ATankPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector DeltaLocation = GetActorForwardVector() * MoveForwardAxisValue * MovementSpeed;
	SetActorLocation(GetActorLocation() + DeltaLocation, false);

	FQuat DeltaRotation = FQuat(FVector(0, 0, 1), MoveRightAxisValue * RotationSpeed);
	AddActorLocalRotation(DeltaRotation, true);
}

// Called to bind functionality to input
void ATankPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &ATankPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATankPawn::MoveRight);

	PlayerInputComponent->BindAction("Shoot", EInputEvent::IE_Pressed, this, &ATankPawn::Shoot);
}

void ATankPawn::HitByBullet(ATankPawn* Enemy)
{
	UE_LOG(LogTemp, Warning, TEXT("Got hit by bullet"));
	Die();
}


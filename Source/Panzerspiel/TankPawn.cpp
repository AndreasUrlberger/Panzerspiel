// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawn.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseBulletActor.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"

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
			FActorSpawnParameters Params;
			Params.Owner = this;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

			FRotator Rotation = GetActorRotation();
			FVector Location = GetBulletSpawnPoint();
			UE_LOG(LogTemp, Warning, TEXT("Spawn bullet, this is : %d"), this);
			ABaseBulletActor* Bullet = World->SpawnActor<ABaseBulletActor>(ToSpawnBullet, Location, Rotation, Params);
			

			// Check if the spawn was successful.
			if (Bullet) {
				UE_LOG(LogTemp, Warning, TEXT("Spawn successfull"));
				Bullet->Init(this);
				++ActiveShots;
				if (FireSound)
					UGameplayStatics::PlaySoundAtLocation(this, FireSound, Location);
			}
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
	return GetActorLocation() + FVector(0, 0, BarrelHeight) + GetActorRotation().Vector() * BarrelLength;
}

void ATankPawn::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Player dies."));
	PlayNiagaraExplosion(GetActorLocation());
	Destroy();
}

void ATankPawn::BulletDestroyed()
{
	check(ActiveShots > 0);
	--ActiveShots;
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
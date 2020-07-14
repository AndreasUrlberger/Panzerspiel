// Fill out your copyright notice in the Description page of Project Settings.


#include "TankPawn.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseBulletActor.h"
#include "Kismet/GameplayStatics.h"
#include "BaseMine.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Controller.h"

// Sets default values
ATankPawn::ATankPawn()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
    TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>("TurretMesh");
    TurretMesh->SetupAttachment(BaseMesh);
    RootComponent = BaseMesh;
}

void ATankPawn::AlignTower(const FVector Target) {
    FVector Rotation = Target - GetActorLocation();
    Rotation.Z = 0;
    
    TurretMesh->SetWorldRotation(Rotation.ToOrientationRotator(), true);
}

void ATankPawn::Shoot()
{    
    if (ActiveShots < MaxShots)
    {
        if (UWorld* World = GetWorld())
        {
            FActorSpawnParameters Params;
            Params.Owner = this;

            FRotator Rotation = TurretMesh->GetComponentRotation();
            FVector Location = GetBulletSpawnPoint();

            ABaseBulletActor* Bullet = World->SpawnActor<ABaseBulletActor>(ToSpawnBullet, Location, Rotation, Params);


            // Check if the spawn was successful.
            if (Bullet)
            {
                ++ActiveShots;
                Bullet->Init(this);
                if (FireSound)
                    UGameplayStatics::PlaySoundAtLocation(this, FireSound, Location);
            }
        }
    }
}

void ATankPawn::PlaceMine() {
    if (ActiveMines < MaxMines)
    {
        if (UWorld* World = GetWorld())
        {
            FActorSpawnParameters Params;
            Params.Owner = this;
   
            // Some kind of random rotation seems to be a good idea.
            FRotator Rotation = TurretMesh->GetComponentRotation();
            FVector Location = GetActorLocation();

            ABaseMine* Mine = World->SpawnActor<ABaseMine>(ToSpawnMine, Location, Rotation, Params);


            // Check if the spawn was successful.
            if (Mine)
            {
                ++ActiveMines;
                Mine->Init(this);
                if (MinePlantSound)
                    UGameplayStatics::PlaySoundAtLocation(this, MinePlantSound, Location);
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
    return TurretMesh->GetComponentLocation() + FVector(0, 0, BarrelHeight) + TurretMesh->GetComponentRotation().Vector() * BarrelLength;
}

void ATankPawn::Die()
{
    PlayNiagaraExplosion(GetActorLocation());
    Destroy();
}

void ATankPawn::BulletDestroyed()
{
    check(ActiveShots > 0);
    --ActiveShots;
}

void ATankPawn::MineDestroyed()
{
    check(ActiveMines > 0);
    --ActiveMines;
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
    PlayerInputComponent->BindAction("PlaceMine", EInputEvent::IE_Pressed, this, &ATankPawn::PlaceMine);
}

void ATankPawn::Kill(ATankPawn* Enemy)
{
    Die();
}

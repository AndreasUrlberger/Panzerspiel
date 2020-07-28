// All rights reserved @Apfelstrudel Games.


#include "TankPawn.h"

#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Bullet.h"
#include "Kismet/GameplayStatics.h"
#include "Mine.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Math/UnrealMathUtility.h"


ATankPawn::ATankPawn()
{
// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
    BaseMesh->SetupAttachment(RootComponent);
    TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>("TurretMesh");
    TurretMesh->SetupAttachment(BaseMesh);
    MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>("MovementComponent");
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
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding;

            const FRotator Rotation = TurretMesh->GetComponentRotation();
            const FVector Location = GetBulletSpawnPoint();

            ABullet* Bullet = World->SpawnActor<ABullet>(ToSpawnBullet, Location, Rotation, Params);


            // Check if the spawn was successful.
            if (Bullet)
            {
                ++ActiveShots;
                Bullet->Init(this);
                if (FireSound)
                    UGameplayStatics::PlaySoundAtLocation(this, FireSound, Location);
            }else {
                // Player tried to shoot directly into a wall -> He dies.
                if(FireSound)
                    UGameplayStatics::PlaySoundAtLocation(this, FireSound, Location);
                Die();
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
            const FRotator Rotation = TurretMesh->GetComponentRotation();
            const FVector Location = GetActorLocation();

            AMine* Mine = World->SpawnActor<AMine>(ToSpawnMine, Location, Rotation, Params);


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

FVector ATankPawn::GetBulletSpawnPoint() const {
    return TurretMesh->GetComponentLocation() + FVector(0, 0, BarrelHeight) + TurretMesh->GetComponentRotation().Vector() * BarrelLength;
}

void ATankPawn::Die()
{
    if(ExplosionSound)
        UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
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

void ATankPawn::MoveAndRotate(const FVector DeltaLocation, const FRotator DeltaRotation) {
    //SetActorLocationAndRotation(GetActorLocation() + DeltaLocation, GetActorRotation() + DeltaRotation);
    SetActorRotation(GetActorRotation() + DeltaRotation);
    MovementComp->AddInputVector(DeltaLocation.GetSafeNormal());
    
}

void ATankPawn::CalculateActualMovement(FVector TargetLocation, float DeltaTime) {
    const FRotator CurrentRotation = GetActorRotation();
    const FRotator TargetDirection = (TargetLocation - GetActorLocation()).Rotation();
    FRotator DeltaRotator = TargetDirection - CurrentRotation;
    DeltaRotator.Normalize();
    float DeltaYaw = DeltaRotator.Yaw;
    UE_LOG(LogTemp, Warning, TEXT("Yaw: %f"), DeltaYaw);

    float MaxYaw = RotationSpeed * DeltaTime;
    if(FMath::Abs(DeltaYaw) <= MaxYaw) {
        // Rotate and move.
        const FVector DeltaLocation = TargetLocation - GetActorLocation();
        MoveAndRotate(DeltaLocation, DeltaRotator);
    }else {
        // Rotate and not move.
        const FRotator DeltaRotation = FRotator(0, FMath::Sign(DeltaYaw) * MaxYaw, 0);
        MoveAndRotate(FVector::ZeroVector, DeltaRotation);
    }
}

// Called every frame
void ATankPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    const FVector DeltaLocation = GetActorForwardVector() * MoveForwardAxisValue * MovementSpeed * DeltaTime;
    FQuat DeltaRotationQuat = FQuat(FVector(0, 0, 1), MoveRightAxisValue * RotationSpeed * DeltaTime);
    MoveAndRotate(DeltaLocation, FRotator(0, MoveRightAxisValue * RotationSpeed * DeltaTime, 0));
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

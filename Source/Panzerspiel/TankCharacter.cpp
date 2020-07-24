// All rights reserved @Apfelstrudel Games.


#include "TankCharacter.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Bullet.h"
#include "Kismet/GameplayStatics.h"
#include "Mine.h"
#include "Engine/EngineTypes.h"

// Sets default values
ATankCharacter::ATankCharacter()
{
// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
    TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>("TurretMesh");
    TurretMesh->SetupAttachment(BaseMesh);
    RootComponent = BaseMesh;
}

void ATankCharacter::AlignTower(const FVector Target) {
    FVector Rotation = Target - GetActorLocation();
    Rotation.Z = 0;
    
    TurretMesh->SetWorldRotation(Rotation.ToOrientationRotator(), true);
}

bool ATankCharacter::MoveTo(FVector TargetLocation, float DeltaTime) {
    UE_LOG(LogTemp, Warning, TEXT("MoveTo: TargetLocation: %s"), *TargetLocation.ToString());
    bool bReachedTarget = false;
    const FVector CurrentLocation = GetActorLocation();
    FVector Route = TargetLocation - CurrentLocation;
    // For some reason the Path tells the object to move up.
    Route.Z = 0;
    FVector Direction;
    float Length;
    Route.ToDirectionAndLength(Direction, Length);

    FVector DeltaMove = Direction * MovementSpeed * DeltaTime;
    if(DeltaMove.SizeSquared2D() - Route.SizeSquared2D() >= 0) {
        // Tank would move too far -> we reached the TargetLocation.
        DeltaMove = Route;
        UE_LOG(LogTemp, Warning, TEXT("Reached Point"));
        bReachedTarget = true;
    }
    SetActorLocation(CurrentLocation + DeltaMove, false);
    SetActorRotation(Direction.Rotation());

    // Tells whether we reached the TargetLocation.
    return bReachedTarget;
}

void ATankCharacter::Shoot()
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

void ATankCharacter::PlaceMine() {
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

void ATankCharacter::MoveForward(float AxisValue)
{
    MoveForwardAxisValue = AxisValue;
}

void ATankCharacter::MoveRight(float AxisValue)
{
    MoveRightAxisValue = AxisValue;
}

FVector ATankCharacter::GetBulletSpawnPoint() const {
    return TurretMesh->GetComponentLocation() + FVector(0, 0, BarrelHeight) + TurretMesh->GetComponentRotation().Vector() * BarrelLength;
}

void ATankCharacter::Die()
{
    if(ExplosionSound)
        UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
    PlayNiagaraExplosion(GetActorLocation());
    Destroy();
}

void ATankCharacter::BulletDestroyed()
{
    check(ActiveShots > 0);
    --ActiveShots;
}

void ATankCharacter::MineDestroyed()
{
    check(ActiveMines > 0);
    --ActiveMines;
}

// Called when the game starts or when spawned
void ATankCharacter::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ATankCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FVector DeltaLocation = GetActorForwardVector() * MoveForwardAxisValue * MovementSpeed * DeltaTime;
    SetActorLocation(GetActorLocation() + DeltaLocation, false);

    FQuat DeltaRotation = FQuat(FVector(0, 0, 1), MoveRightAxisValue * RotationSpeed * DeltaTime);
    AddActorLocalRotation(DeltaRotation, true);
}

// Called to bind functionality to input
void ATankCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis("MoveForward", this, &ATankCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ATankCharacter::MoveRight);

    PlayerInputComponent->BindAction("Shoot", EInputEvent::IE_Pressed, this, &ATankCharacter::Shoot);
    PlayerInputComponent->BindAction("PlaceMine", EInputEvent::IE_Pressed, this, &ATankCharacter::PlaceMine);
}

void ATankCharacter::Kill(ATankCharacter* Enemy)
{
    Die();
}



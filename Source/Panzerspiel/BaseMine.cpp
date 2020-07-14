// All rights reserved Apfelstrudel Games.


#include "BaseMine.h"

#include "TankPawn.h"
#include "Components/SphereComponent.h"
#include "BaseBulletActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseMine::ABaseMine() {
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    MineMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mine Mesh");
    SetRootComponent(MineMesh);

    ExplosionMesh = CreateDefaultSubobject<UStaticMeshComponent>("ExplosionMesh");
    ExplosionMesh->SetupAttachment(RootComponent);
    ExplosionMesh->Deactivate();

    TankTriggerSphere = CreateDefaultSubobject<USphereComponent>("TankTriggerSphere");
    TankTriggerSphere->SetupAttachment(RootComponent);

    BulletTriggerSphere = CreateDefaultSubobject<USphereComponent>("BulletTriggerSphere");
    BulletTriggerSphere->SetupAttachment(RootComponent);

    KillSphere = CreateDefaultSubobject<USphereComponent>("KillSphere");
    KillSphere->SetupAttachment(RootComponent);
    KillSphere->SetSphereRadius(0);
}


void ABaseMine::Init(ATankPawn* SourcePawn) {
    this->TankPawn = SourcePawn;
}


// Called when the game starts or when spawned
void ABaseMine::BeginPlay() {
    Super::BeginPlay();

    TankTriggerSphere->SetSphereRadius(0);
    BulletTriggerSphere->SetSphereRadius(0);
}

// Called every frame
void ABaseMine::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    if (!MineActive) {
        ActivationTime -= DeltaTime;
        if (ActivationTime <= 0) {
            Activate();
        }
    }

    if (ExplosionRunning) {
        RunningTime += DeltaTime;
        float Scale = RunningTime / ExplosionTime * ExplosionRadius;

        if (Scale >= ExplosionRadius) {
            Scale = ExplosionRadius;
            ExplosionRunning = false;
            ExplosionAtMax = true;
            KillSphere->OnComponentBeginOverlap.RemoveAll(this);
            MineMesh->Deactivate();
        }

        KillSphere->SetSphereRadius(Scale, true);
        ExplosionMesh->SetWorldScale3D(FVector(Scale, Scale, Scale));
    }
    if (ExplosionAtMax) {
        CurrentTimeAtMax += DeltaTime;
        if (CurrentTimeAtMax >= TimeAtMax)
            Die();
    }
}

void ABaseMine::BeginTankOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult) {

    if (!ExplosionRunning && Cast<ATankPawn>(OtherActor)) {
        Explode();
    }

}

void ABaseMine::BeginBulletOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                        const FHitResult& SweepResult) {

    if (!ExplosionRunning && Cast<ABaseBulletActor>(OtherActor)) {
        Explode();
    }
}

void ABaseMine::BeginKillOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp,
                                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
    if (ATankPawn* OtherTank = Cast<ATankPawn>(OtherActor))
        OtherTank->Kill(TankPawn);
    else if (ABaseBulletActor* Bullet = Cast<ABaseBulletActor>(OtherActor))
        Bullet->Kill(TankPawn);
    else if (ABaseMine* Mine = Cast<ABaseMine>(OtherActor)) {
        Mine->Explode();
    }
}

void ABaseMine::Explode() {
    // Explode might get called multiple times by other actors.
    if(Triggered)
        return;
    Triggered = true;
    ExplosionRunning = true;
    // If a mine explodes it is also active, we set this so that the TriggerCollisions do not get activated in tick().
    MineActive = true;

    // We do not need them from here on.
    TankTriggerSphere->Deactivate();
    BulletTriggerSphere->Deactivate();
    
    TankPawn->MineDestroyed();
    KillSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseMine::BeginKillOverlapEvent);
    TankTriggerSphere->OnComponentBeginOverlap.RemoveAll(this);
    BulletTriggerSphere->OnComponentBeginOverlap.RemoveAll(this);
}

void ABaseMine::Die() {
    Destroy();
}

void ABaseMine::Activate() {
    MineActive = true;
    if (ActivationSound)
        UGameplayStatics::PlaySoundAtLocation(this, ActivationSound, GetActorLocation());

    TankTriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseMine::BeginTankOverlapEvent);
    BulletTriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseMine::BeginBulletOverlapEvent);

    // Its important to set the radius after the AddDynamic because otherwise we would not get the initial overlap.
    TankTriggerSphere->SetSphereRadius(TankTriggerRadius);
    BulletTriggerSphere->SetSphereRadius(BulletTriggerRadius);
}

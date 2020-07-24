// All rights reserved @Apfelstrudel Games.


#include "Mine.h"
#include "TankCharacter.h"
#include "Bullet.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
AMine::AMine() {
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    MineMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mine Mesh");
    SetRootComponent(MineMesh);

    ExplosionMesh = CreateDefaultSubobject<UStaticMeshComponent>("ExplosionMesh");
    ExplosionMesh->SetupAttachment(RootComponent);
    ExplosionMesh->Deactivate();

    // We move all collision spheres under the map before we activate them to make sure we receive every overlap event.
    TankTriggerSphere = CreateDefaultSubobject<USphereComponent>("TankTriggerSphere");
    TankTriggerSphere->SetupAttachment(RootComponent);
    TankTriggerSphere->SetRelativeLocation(FVector(0, 0, -2*TankTriggerRadius));

    BulletTriggerSphere = CreateDefaultSubobject<USphereComponent>("BulletTriggerSphere");
    BulletTriggerSphere->SetupAttachment(RootComponent);
    BulletTriggerSphere->SetRelativeLocation(FVector(0, 0, -2*BulletTriggerRadius));

    KillSphere = CreateDefaultSubobject<USphereComponent>("KillSphere");
    KillSphere->SetupAttachment(RootComponent);
    KillSphere->SetSphereRadius(0);
    KillSphere->SetRelativeLocation(FVector(0, 0, -2*ExplosionRadius));
}


void AMine::Init(ATankCharacter* SourcePawn) {
    this->TankCharacter = SourcePawn;
}


// Called when the game starts or when spawned
void AMine::BeginPlay() {
    Super::BeginPlay();

    TankTriggerSphere->SetSphereRadius(0);
    BulletTriggerSphere->SetSphereRadius(0);

    UMaterialInterface* Material = MineMesh->GetMaterial(0);
    DynamicMineMaterial = UMaterialInstanceDynamic::Create(Material, nullptr);
    MineMesh->SetMaterial(0, DynamicMineMaterial);
}

// Called every frame
void AMine::Tick(float DeltaTime) {
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

void AMine::BeginTankOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
    if (!ExplosionRunning && Cast<ATankCharacter>(OtherActor))
        Explode();
}

void AMine::BeginBulletOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
    if (!ExplosionRunning && Cast<ABullet>(OtherActor))
        Explode();
}

void AMine::BeginKillOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
    if (ATankCharacter* OtherTank = Cast<ATankCharacter>(OtherActor))
        OtherTank->Kill(TankCharacter);
    else if (ABullet* Bullet = Cast<ABullet>(OtherActor))
        Bullet->Kill(TankCharacter);
    else if (AMine* Mine = Cast<AMine>(OtherActor)) {
        Mine->Explode();
    }
}

void AMine::Explode() {
    // Explode might get called multiple times by other actors.
    if(Triggered)
        return;
    Triggered = true;
    if(ExplosionSound)
        UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
    ExplosionRunning = true;
    // If a mine explodes it is also active, we set this so that the TriggerCollisions do not get activated in tick().
    MineActive = true;

    // We do not need them from here on.
    TankTriggerSphere->Deactivate();
    BulletTriggerSphere->Deactivate();
    
    TankCharacter->MineDestroyed();
    KillSphere->OnComponentBeginOverlap.AddDynamic(this, &AMine::BeginKillOverlapEvent);
    KillSphere->SetRelativeLocation(FVector(0, 0, 0), true);
    TankTriggerSphere->OnComponentBeginOverlap.RemoveAll(this);
    BulletTriggerSphere->OnComponentBeginOverlap.RemoveAll(this);
}

void AMine::Die() {
    Destroy();
}

void AMine::Activate() {
    MineActive = true;
    // Parameter 1 needs to be the defining the color blend.
    DynamicMineMaterial->SetScalarParameterValue("ActiveColorBlend", 1);
    
    if (ActivationSound)
        UGameplayStatics::PlaySoundAtLocation(this, ActivationSound, GetActorLocation());

    TankTriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AMine::BeginTankOverlapEvent);
    BulletTriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AMine::BeginBulletOverlapEvent);

    // Its important to set the radius after the AddDynamic because otherwise we would not get the initial overlap.
    TankTriggerSphere->SetSphereRadius(TankTriggerRadius);
    BulletTriggerSphere->SetSphereRadius(BulletTriggerRadius);
    TankTriggerSphere->SetRelativeLocation(FVector(0, 0, 0), true);
    BulletTriggerSphere->SetRelativeLocation(FVector(0, 0, 0), true);
}

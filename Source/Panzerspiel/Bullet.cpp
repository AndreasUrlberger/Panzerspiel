// All rights reserved @Apfelstrudel Games.


#include "Bullet.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TankCharacter.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ABullet::ABullet() {
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    CollisionComp = CreateDefaultSubobject<UBoxComponent>("Collision Component");
    RootComponent = CollisionComp;

    BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("Bullet Mesh");
    BulletMesh->SetupAttachment(RootComponent);

    ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Component");
    ProjectileComp->UpdatedComponent = CollisionComp;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay() {
    Super::BeginPlay();
    CollisionComp->OnComponentHit.AddDynamic(this, &ABullet::HitEvent);
    CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABullet::BeginOverlapEvent);
    CollisionComp->OnComponentEndOverlap.AddDynamic(this, &ABullet::EndOverlapEvent);
}

// Called every frame
void ABullet::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    // We check for overlaps every frame as long as we currently are overlapping something.
    if(FirstOverlapEventActor)
        BeginOverlapEvent(nullptr, FirstOverlapEventActor, nullptr, 0, false, FHitResult());
}

void ABullet::Init(ATankCharacter* Spawner) {
    Source = Spawner;
    SourceVulnerable = false;
    // Collisions had to be disabled up to this point.
    SetActorEnableCollision(true);
    // Need to call OverlapEvent since it might wrongly interpreted the first overlap event because Source was null.
    BeginOverlapEvent(nullptr, FirstOverlapEventActor, nullptr, 0, false, FHitResult());
}

void ABullet::Kill(ATankCharacter* Enemy) {
    Die();
}

void ABullet::HitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit) {
    SourceVulnerable = true;
    if (HitsBeforeDeath > 0) {
        // Bullet hit wall.
        --HitsBeforeDeath;
        if (WallHitSound)
            UGameplayStatics::PlaySoundAtLocation(this, WallHitSound, GetActorLocation());
    } else {
        Die();
    }

}

void ABullet::BeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
    // Save the OtherActor from the first call since we need to handle it once Init got called.
    FirstOverlapEventActor = OtherActor;
    // We can only handle overlaps properly if we know the owner of the bullet.
    if(!Source)
        return;

    if (Cast<ABullet>(OtherActor)) {
        Die();
    } else if (ATankCharacter* HitTank = Cast<ATankCharacter>(OtherActor)) {
        if (HitTank != Source || SourceVulnerable) {
            HitTank->Kill(Source);
            Die();
        }
    }
}

void ABullet::EndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
    FirstOverlapEventActor = nullptr;
}


void ABullet::Die() {
    // This bullet is already dead if ProjectileComp is null.
    if(!ProjectileComp)
        return;
    
    if (BulletDestroySound)
        UGameplayStatics::PlaySoundAtLocation(this, BulletDestroySound, GetActorLocation());

    // Stops the bullets movement.
    ProjectileComp->DestroyComponent();
    ProjectileComp = nullptr;
    CollisionComp->DestroyComponent();
    CollisionComp = nullptr;
    BulletMesh->DestroyComponent();
    BulletMesh = nullptr;

    // Tell the blueprint to stop the smoke emitter.
    StopSmoke();

    // This bullet is "officially" destroyed but technically still lives.
    if (Source)
        Source->BulletDestroyed();
}

void ABullet::FinalDie() {
    Destroy();
}

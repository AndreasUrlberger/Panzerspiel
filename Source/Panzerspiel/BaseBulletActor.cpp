// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBulletActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TankPawn.h"

// Sets default values
ABaseBulletActor::ABaseBulletActor() {
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    CollisionComp = CreateDefaultSubobject<UBoxComponent>("Collision Component");
    RootComponent = CollisionComp;

    BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("Bullet Mesh");
    BulletMesh->SetupAttachment(RootComponent);

    ProjectileComp = CreateDefaultSubobject<UProjectileMovementComponent>("Projectile Component");
    ProjectileComp->UpdatedComponent = CollisionComp;

    // Disable collisions so that init can be called before a collision gets triggered as we need Source to be initialized.
    SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void ABaseBulletActor::BeginPlay() {
    Super::BeginPlay();
    CollisionComp->OnComponentHit.AddDynamic(this, &ABaseBulletActor::HitEvent);
    CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseBulletActor::BeginOverlapEvent);
    CollisionComp->OnComponentEndOverlap.AddDynamic(this, &ABaseBulletActor::EndOverlapEvent);
}

// Called every frame
void ABaseBulletActor::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);
    // We check for overlaps every frame as long as we currently are overlapping something.
    if(FirstOverlapEventActor)
        BeginOverlapEvent(nullptr, FirstOverlapEventActor, nullptr, 0, false, FHitResult());
}

void ABaseBulletActor::Init(ATankPawn* Spawner) {
    Source = Spawner;
    SourceVulnerable = false;
    // Collisions had to be disabled up to this point.
    SetActorEnableCollision(true);
    // Need to call OverlapEvent since it might wrongly interpreted the first overlap event because Source was null.
    BeginOverlapEvent(nullptr, FirstOverlapEventActor, nullptr, 0, false, FHitResult());
}

void ABaseBulletActor::HitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
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

void ABaseBulletActor::BeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
    FirstOverlapEventActor = OtherActor;

    if (Cast<ABaseBulletActor>(OtherActor)) {
        Die();
    } else if (ATankPawn* HitTank = Cast<ATankPawn>(OtherActor)) {
        if (HitTank != Source || SourceVulnerable) {
            HitTank->HitByBullet(Source);
            Die();
        }
    }
}

void ABaseBulletActor::EndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
    FirstOverlapEventActor = nullptr;
}


void ABaseBulletActor::Die() {
    if (BulletDestroySound)
        UGameplayStatics::PlaySoundAtLocation(this, BulletDestroySound, GetActorLocation());
    if (Source)
        Source->BulletDestroyed();
    Destroy();
}

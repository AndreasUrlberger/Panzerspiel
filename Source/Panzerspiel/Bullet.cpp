// All rights reserved @Apfelstrudel Games.


#include "Bullet.h"

#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TankPawn.h"
#include "Particles/ParticleSystemComponent.h"
#include "Utility.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnitConversion.h"

// Sets default values
ABullet::ABullet() {
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    CollisionComp = CreateDefaultSubobject<UBoxComponent>("Collision Component");
    RootComponent = CollisionComp;

    BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>("Bullet Mesh");
    BulletMesh->SetupAttachment(RootComponent);

    RicochetPoint = CreateDefaultSubobject<USceneComponent>("RicochetPoint");
    RicochetPoint->SetupAttachment(RootComponent);
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

    BulletMove(DeltaTime);
}

void ABullet::Init(ATankPawn* Spawner) {
    Source = Spawner;
    SourceVulnerable = false;
    // Collisions had to be disabled up to this point.
    SetActorEnableCollision(true);
    // Need to call OverlapEvent since it might wrongly interpreted the first overlap event because Source was null.
    BeginOverlapEvent(nullptr, FirstOverlapEventActor, nullptr, 0, false, FHitResult());
}

void ABullet::Kill(ATankPawn* Enemy) {
    Die();
}

void ABullet::HitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                FVector NormalImpulse, const FHitResult& Hit) {
    UE_LOG(LogTemp, Warning, TEXT("HitEvent"));
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
    UE_LOG(LogTemp, Warning, TEXT("BeginOverlapEvent"));
    // Save the OtherActor from the first call since we need to handle it once Init got called.
    FirstOverlapEventActor = OtherActor;
    // We can only handle overlaps properly if we know the owner of the bullet.
    if(!Source)
        return;

   
    if (Cast<ABullet>(OtherActor)) {
        Die();
    } else if (ATankPawn* HitTank = Cast<ATankPawn>(OtherActor)) {
        if (HitTank != Source || SourceVulnerable) {
            HitTank->Kill(Source);
            Die();
        }
    }// else if(OtherActor == Obstacle -> We dont care since BulletMove already covers that.
}

void ABullet::EndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
    FirstOverlapEventActor = nullptr;
}

// TODO: Bullet is currently kinda immune against collisions while sliding.
void ABullet::BulletMove(const float DeltaTime) {
    if(bIsDead) return;

    // Do LineTrace.
    const float DistanceToTravel = DeltaTime * Speed;
    CalculateMove(DistanceToTravel, this);
}

void ABullet::CalculateMove(const float DistanceToMove, const AActor* IgnoreActor) {
    UWorld* World = GetWorld();
    if(!World) return;
    
    FHitResult Result;
    const FVector StartLoc = FVector(GetActorLocation().X, GetActorLocation().Y, TraceHeight);
    const FVector EndLoc = StartLoc + GetActorForwardVector() * DistanceToMove;
    ECollisionChannel Channel = ECC_PhysicsBody;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(IgnoreActor);
    World->LineTraceSingleByChannel(Result, StartLoc, EndLoc, Channel, Params);

    // Evaluate LineTrace.
    if(AActor* HitActor = Result.GetActor()) {
        // Collided with something.
        UE_LOG(LogTemp, Warning, TEXT("Collided with something while planning the move."));
        if(Cast<AWorldObstacle>(HitActor)) {
            UE_LOG(LogTemp, Warning, TEXT("Bullet hit a WorldObstacle."));
            // Ricochet.
            --HitsBeforeDeath;
            if(HitsBeforeDeath <= 0)
                Die();
            else {
                if(WallHitSound)
                    UGameplayStatics::PlaySoundAtLocation(World, WallHitSound, Result.ImpactPoint);
                // Begin recursion.
                FVector2D NewDirection2D = UUtility::MirrorVector(FVector2D(GetActorForwardVector()), FVector2D(Result.ImpactPoint), FVector2D(Result.ImpactNormal));
                SetActorLocation(GetActorLocation() + GetActorForwardVector() * Result.Distance);
                SetActorRotation(FVector(NewDirection2D.X, NewDirection2D.Y, 0).Rotation());
                CalculateMove(DistanceToMove - Result.Distance, Result.GetActor());
            }
        }else if(ATankPawn* TankPawn = Cast<ATankPawn>(HitActor)) {
            // Hit a tank -> trigger tank collision.
            UE_LOG(LogTemp, Warning, TEXT("Bullet calculated that it will hit a tank."));
        }else if(ABullet* Bullet = Cast<ABullet>(HitActor)){
            UE_LOG(LogTemp, Warning, TEXT("Bullet hits another bullet"));
        }else {
            UE_LOG(LogTemp, Warning, TEXT("Bullet hit something unknown."));
        }
    }else {
        // No Collision -> Move there.
        SetActorLocation(EndLoc);
    }
}

void ABullet::Die() {
    if(bIsDead)
        return;
    
    if (BulletDestroySound)
        UGameplayStatics::PlaySoundAtLocation(this, BulletDestroySound, GetActorLocation());

    // Stops the bullets movement.
    bIsDead = true;
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
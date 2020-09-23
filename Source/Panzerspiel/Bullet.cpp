// All rights reserved @Apfelstrudel Games.


#include "Bullet.h"


#include "DrawDebugHelpers.h"
#include "PropertyEditorModule.h"
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


void ABullet::BulletMove(const float DeltaTime) {
    if(bIsDead) return;
    UWorld* World = GetWorld();
    if(!World) return;

    float DistanceToTravel = DeltaTime * Speed;
    if(bIsSliding) {
        // Continue slide.
        // Move along edge.
        float DistanceAlongEdge = (SlideEndPoint - GetActorLocation()).Size();
        if(DistanceAlongEdge > DistanceToTravel) {
            FVector NewLoc = FMath::VInterpConstantTo(GetActorLocation(), SlideEndPoint, DeltaTime, Speed);
            SetActorLocation(NewLoc);
            SetActorRotation(GetActorForwardVector().Rotation()); // Currently redundant.
            bIsSliding = true; // redundant.
            return;
        }else {
            SetActorLocation(SlideEndPoint);
            SetActorRotation(GetActorForwardVector().Rotation());
            DistanceToTravel -= DistanceAlongEdge;
            bIsSliding = true; // redundant.
        }

        // Move along mirrored direction.
        SetActorRotation(NextDirection.Rotation());
        FVector EndPos = GetActorLocation() + DistanceToTravel * NextDirection;
        SetActorLocation(EndPos);
        bIsSliding = false;
        return;
    }
    FVector DeltaLoc = DistanceToTravel * GetActorForwardVector();
    FVector TargetLoc = GetActorLocation() + DeltaLoc;
    // Try to move he bullet normally.
    FHitResult Result;
    SetActorLocation(TargetLoc, true, &Result);

    if(!Result.GetActor()) // Did not collide with anything thus we are already finished.
        return;

    if(Cast<AWorldObstacle>(Result.GetActor())) {
        // Account for the already traveled distance.
        DistanceToTravel -= Result.Distance;
        // Calculate necessary locations and directions.
        const float ActorHeight = GetActorLocation().Z;
        FVector EdgeNormal = Result.ImpactNormal;
        FVector EdgeDirection = FVector(EdgeNormal.Y, -EdgeNormal.X, 0);
        if((EdgeDirection | GetActorForwardVector()) < 0) // If true the EdgeDirection goes against the bullet direction.
            EdgeDirection *= -1;
        FVector2D SupposedHitPoint2D = UUtility::CalculateIntersect(FVector2D(Result.ImpactPoint), FVector2D(EdgeDirection), FVector2D(GetActorLocation()), FVector2D(DeltaLoc));
        // Project actor location onto edge to get the edge entry point.
        const FVector2D PointFromOrigin = FVector2D(GetActorLocation()) - SupposedHitPoint2D;
        const FVector2D LineEntryPoint = FVector2D(EdgeDirection) * (PointFromOrigin | FVector2D(EdgeDirection)) + SupposedHitPoint2D;
        const FVector RealEntryPoint = FVector(LineEntryPoint.X + EdgeNormal.X * BulletWidth, LineEntryPoint.Y + EdgeNormal.Y * BulletWidth, ActorHeight);
        const FVector2D LineExitPoint = SupposedHitPoint2D + (SupposedHitPoint2D - LineEntryPoint);
        SlideEndPoint = FVector(LineExitPoint.X + EdgeNormal.X * BulletWidth, LineExitPoint.Y + EdgeNormal.Y * BulletWidth, ActorHeight);
        FVector2D MirroredDirection = UUtility::MirrorVector(FVector2D(-GetActorForwardVector()), SupposedHitPoint2D, FVector2D(EdgeNormal));
        DrawDebugLine(World, FVector(SupposedHitPoint2D.X, SupposedHitPoint2D.Y, 0), FVector(SupposedHitPoint2D.X, SupposedHitPoint2D.Y, 0) + 100 * FVector(MirroredDirection.X, MirroredDirection.Y, 0), FColor::Blue, true, -1, 0, 3);
        NextDirection = FVector(MirroredDirection.X, MirroredDirection.Y, 0).GetUnsafeNormal();

        // Now that we have the entry, exit and direction we now have to move it along those points.
        // Move along direction till entry point. (Useless for pretty much any case thus I might remove it.)
        // TODO: Remove this part.
        float DistanceToEntry = (RealEntryPoint - GetActorLocation()).Size();
        if(DistanceToEntry > DistanceToTravel) {
            // We are done after this step.
            FVector NewLoc = FMath::VInterpConstantTo(GetActorLocation(), RealEntryPoint, DeltaTime, Speed);
            SetActorLocation(NewLoc);
            SetActorRotation(EdgeDirection.Rotation());
            bIsSliding = true;
            return;
        }else {
            SetActorLocation(RealEntryPoint);
            SetActorRotation(EdgeDirection.Rotation());
            DistanceToTravel -= DistanceToEntry;
            bIsSliding = true;
        }

        // Move along edge.
        float DistanceAlongEdge = (SlideEndPoint - RealEntryPoint).Size();
        if(DistanceAlongEdge > DistanceToTravel) {
            FVector NewLoc = FMath::VInterpConstantTo(GetActorLocation(), SlideEndPoint, DeltaTime, Speed);
            SetActorLocation(NewLoc);
            SetActorRotation(EdgeDirection.Rotation()); // Currently redundant.
            bIsSliding = true; // redundant.
            return;
        }else {
            SetActorLocation(SlideEndPoint);
            SetActorRotation(EdgeDirection.Rotation());
            DistanceToTravel -= DistanceAlongEdge;
            bIsSliding = true; // redundant.
        }

        // Move along mirrored direction.
        SetActorRotation(NextDirection.Rotation());
        FVector EndPos = GetActorLocation() + DistanceToTravel * NextDirection;
        SetActorLocation(EndPos);
        bIsSliding = false;
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
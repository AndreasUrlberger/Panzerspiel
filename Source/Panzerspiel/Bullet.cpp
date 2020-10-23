// All rights reserved @Apfelstrudel Games.

#ifndef COLLISION_BULLET_TRACE
#define COLLISION_BULLET_TRACE ECC_GameTraceChannel3
#endif

#include "Bullet.h"

#include "Mine.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TankPawn.h"
#include "Particles/ParticleSystemComponent.h"
#include "Utility.h"
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
}

// Called every frame
void ABullet::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	BulletMove(DeltaTime);
}

void ABullet::Init(ATankPawn* Spawner) {
	Source = Spawner;
}

void ABullet::Kill(ATankPawn* Enemy) {
	Die();
}

void ABullet::BulletMove(const float DeltaTime) {
	if (bIsDead) return;

	// Do LineTrace.
	const float DistanceToTravel = DeltaTime * Speed;
	CalculateMove(DistanceToTravel, this);
}

bool ABullet::TankHitEvent(ATankPawn* OtherTank) {
	// We can only handle overlaps properly if we know the owner of the bullet.
	if (!Source)
		return false; // Ignore this tank.

	if(OtherTank == Source && !SourceVulnerable) // Ignore this tank.
		return false;
	
	OtherTank->Kill(Source);
	Die();
	return true; // Killed this tank.
}

void ABullet::BulletHitEvent(ABullet* OtherBullet) {
	if (IsValid(OtherBullet))
		OtherBullet->Die(); // Otherwise the other bullet might not know of the collision.
	Die();
}

void ABullet::CalculateMove(const float DistanceToMove, const AActor* IgnoreActor) {
	UWorld* World = GetWorld();
	if (!World) return;

	FHitResult Result;
	//const FVector StartLoc = FVector(GetActorLocation().X, GetActorLocation().Y, TraceHeight);
	const FVector StartLoc = GetActorLocation();
	const FVector EndLoc = StartLoc + GetActorForwardVector() * DistanceToMove;
	ECollisionChannel Channel = COLLISION_BULLET_TRACE;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(IgnoreActor);
	Params.AddIgnoredActor(this);
	World->LineTraceSingleByChannel(Result, StartLoc, EndLoc, Channel, Params);

	// Evaluate LineTrace.
	if (AActor* HitActor = Result.GetActor()) {
		// Collided with something.
		if (Cast<AWorldObstacle>(HitActor)) {
			SourceVulnerable = true; // The shooter can only get killed by the own bullet once it hit something else.
			// Ricochet.
			--HitsBeforeDeath;
			if (HitsBeforeDeath < 0)
				Die();
			else {
				if (WallHitSound)
					UGameplayStatics::PlaySoundAtLocation(World, WallHitSound, Result.ImpactPoint);
				// Begin recursion.
				FVector2D NewDirection2D = UUtility::MirrorVector(FVector2D(GetActorForwardVector()),
					FVector2D(Result.ImpactPoint), FVector2D(Result.ImpactNormal.Y,
				          -Result.ImpactNormal.X));
				SetActorLocation(GetActorLocation() + GetActorForwardVector() * Result.Distance);
				SetActorRotation(FVector(NewDirection2D.X, NewDirection2D.Y, 0).Rotation());
				CalculateMove(DistanceToMove - Result.Distance, Result.GetActor());
			}
		} else if (ATankPawn* HitTank = Cast<ATankPawn>(HitActor)) {
			// Hit a tank -> trigger tank collision.
			if(!TankHitEvent(HitTank)) {
				// Continue bullet movement but ignore the hit tank.
				SetActorLocation(GetActorLocation() + GetActorForwardVector() * Result.Distance);
				CalculateMove(DistanceToMove - Result.Distance, HitTank);
			}

		} else if (ABullet* Bullet = Cast<ABullet>(HitActor)) {
			// Bullet hits another bullet.
			BulletHitEvent(Bullet);
		}else if(Cast<AMine>(HitActor)){
			// Continue as if there was nothing to trigger the mine's hit event.
			SetActorLocation(EndLoc);
		} else {
			UE_LOG(LogTemp, Error,
			       TEXT(
				       "Bullet hit something unknown. If this happens in the finished game in this level is a object that sould not be there."
			       ));
			Die(); // Just make sure it dies.
		}
	} else {
		// No Collision -> Move there.
		SetActorLocation(EndLoc);
	}
}

void ABullet::Die() {
	if (bIsDead)
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBulletActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TankPawn.h"

// Sets default values
ABaseBulletActor::ABaseBulletActor()
{
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
void ABaseBulletActor::BeginPlay()
{
	Super::BeginPlay();
	CollisionComp->OnComponentHit.AddDynamic(this, &ABaseBulletActor::HitEvent);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseBulletActor::OverlapEvent);
}

// Called every frame
void ABaseBulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseBulletActor::Init(ATankPawn* Spawner)
{
	this->Source = Source;
}

void ABaseBulletActor::HitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	if (HitsBeforeDeath > 0) {
		--HitsBeforeDeath;
		if (WallHitSound)
			UGameplayStatics::PlaySoundAtLocation(this, WallHitSound, GetActorLocation());
	}
	else {
		Die();
	}
	
}

void ABaseBulletActor::OverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ABaseBulletActor>(OtherActor)) {
		Die();
	}
	else if (ATankPawn* HitTank = Cast<ATankPawn>(OtherActor)) {
		HitTank->HitByBullet(Source);
		Die();
	}
}

void ABaseBulletActor::Die() {
	if (BulletDestroySound) {
		UGameplayStatics::PlaySoundAtLocation(this, BulletDestroySound, GetActorLocation());
	}
	Destroy();
}

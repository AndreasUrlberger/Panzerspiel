// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBulletActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

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
}

// Called every frame
void ABaseBulletActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseBulletActor::HitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	if(WallHitSound) {
		UGameplayStatics::PlaySoundAtLocation(this, WallHitSound, GetActorLocation());
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No sound cue set"));
	}
}

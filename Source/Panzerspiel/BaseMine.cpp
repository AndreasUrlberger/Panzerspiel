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
	TankTriggerSphere->SetSphereRadius(TankTriggerRadius);
	BulletTriggerSphere->SetSphereRadius(BulletTriggerRadius);
	TankTriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseMine::BeginTriggerOverlapEvent);
	BulletTriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseMine::BeginTriggerOverlapEvent);
	TankTriggerSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseMine::EndTriggerOverlapEvent);
	BulletTriggerSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseMine::EndTriggerOverlapEvent);
}

// Called every frame
void ABaseMine::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if(OverlapActor)
		BeginTriggerOverlapEvent(nullptr, OverlapActor, nullptr, 0, false, FHitResult());
	
	if(!MineActive) {
		ActivationTime -= DeltaTime;
		if (ActivationTime <= 0) {
			MineActive = true;
			if(ActivationSound)
				UGameplayStatics::PlaySoundAtLocation(this, ActivationSound, GetActorLocation());
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

void ABaseMine::BeginTriggerOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult) {
	OverlapActor = OtherActor;
	if (MineActive && !ExplosionRunning && Cast<ATankPawn>(OtherActor)) {
		// We do not need them from here on.
		TankTriggerSphere->Deactivate();
		BulletTriggerSphere->Deactivate();
		Explode();
	}
}

void ABaseMine::EndTriggerOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	OverlapActor = nullptr;
}
	

void ABaseMine::BeginKillOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp,
                                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if(ATankPawn* OtherTank = Cast<ATankPawn>(OtherActor))
		OtherTank->Kill(TankPawn);
	else if(ABaseBulletActor* Bullet = Cast<ABaseBulletActor>(OtherActor))
		Bullet->Kill(TankPawn);
}

void ABaseMine::Explode() {
	ExplosionRunning = true;
	TankPawn->MineDestroyed();
	KillSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseMine::BeginKillOverlapEvent);
	TankTriggerSphere->OnComponentBeginOverlap.RemoveAll(this);
	BulletTriggerSphere->OnComponentBeginOverlap.RemoveAll(this);
}

void ABaseMine::Die() {
	Destroy();
}

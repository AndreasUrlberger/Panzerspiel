// All rights reserved Apfelstrudel Games.


#include "BaseMine.h"

#include "TankPawn.h"
#include "Components/SphereComponent.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
ABaseMine::ABaseMine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MineMesh = CreateDefaultSubobject<UStaticMeshComponent>("Mine Mesh");
	SetRootComponent(MineMesh);

	ExplosionMesh = CreateDefaultSubobject<UStaticMeshComponent>("ExplosionMesh");
	ExplosionMesh->SetupAttachment(RootComponent);
	ExplosionMesh->Deactivate();

	TankTriggerSphere = CreateDefaultSubobject<USphereComponent>("TankTriggerSphere");
	TankTriggerSphere->SetupAttachment(RootComponent);
	TankTriggerSphere->SetSphereRadius(TankTriggerRadius);

	BulletTriggerSphere = CreateDefaultSubobject<USphereComponent>("BulletTriggerSphere");
	BulletTriggerSphere->SetupAttachment(RootComponent);
	BulletTriggerSphere->SetSphereRadius(BulletTriggerRadius);

	KillSphere = CreateDefaultSubobject<USphereComponent>("KillSphere");
	KillSphere->SetupAttachment(RootComponent);
	KillSphere->SetSphereRadius(0);
}

// Called when the game starts or when spawned
void ABaseMine::BeginPlay()
{
	Super::BeginPlay();
	TankTriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseMine::BeginTankOverlapEvent);
}

// Called every frame
void ABaseMine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ActivationTime -= DeltaTime;
	if(ActivationTime <= 0)
		MineActive = true;

	if(ExplosionRunning) {
		RunningTime += DeltaTime;
		const float Scale = FMath::Clamp(RunningTime/ExplosionTime, 0.f, 1.f) * ExplosionRadius;
		KillSphere->SetSphereRadius(Scale);
		ExplosionMesh->SetWorldScale3D(FVector(Scale, Scale, Scale));
	}
}


void ABaseMine::BeginTankOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	if(MineActive && !ExplosionRunning && Cast<ATankPawn>(OtherActor)) {
		UE_LOG(LogTemp, Warning, TEXT("Triggered by Tank"));
		// We do not need them from here on.
		TankTriggerSphere->Deactivate();
		BulletTriggerSphere->Deactivate();
		Explode();
	}
}

void ABaseMine::BeginKillOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	UE_LOG(LogTemp, Warning, TEXT("Triggered BeginKillOverlapEvent"));
}

void ABaseMine::Explode() {
	UE_LOG(LogTemp, Warning, TEXT("Called Explode"));
	ExplosionRunning = true;
	KillSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseMine::BeginKillOverlapEvent);
}


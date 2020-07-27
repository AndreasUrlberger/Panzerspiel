// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mine.generated.h"

UCLASS()
class PANZERSPIEL_API AMine : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* MineMesh;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* ExplosionMesh;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* TankTriggerSphere;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* BulletTriggerSphere;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* KillSphere;

	UPROPERTY(EditAnywhere)
	float ExplosionRadius = 100;

	// ExplosionTime in seconds.
	UPROPERTY(EditAnywhere)
	float ExplosionTime = 0.25;

	UPROPERTY(EditAnywhere)
	float TimeAtMax = 0.1;

	UPROPERTY(VisibleAnywhere)
	float CurrentTimeAtMax;

	UPROPERTY(VisibleAnywhere)
	bool ExplosionAtMax;
	
	UPROPERTY(VisibleAnywhere)
	bool MineActive;

	UPROPERTY(EditAnywhere)
	float BulletTriggerRadius = 25;

	UPROPERTY(EditAnywhere)
	float TankTriggerRadius = 50;

	// Activation time in seconds.
	UPROPERTY(EditAnywhere)
	float ActivationTime = 1;

	UPROPERTY(VisibleAnywhere)
	bool ExplosionRunning;

	// Is set true once Explode got called an wont be set to false at any time after that. With this bool we can make
	// sure that Explode will never be called two times.
	UPROPERTY(VisibleAnywhere)
	bool Triggered;

	UPROPERTY(VisibleAnywhere)
	float RunningTime = 0;

	UPROPERTY(VisibleAnywhere)
	class ATankPawn* TankPawn;

	UPROPERTY(EditAnywhere)
	class USoundBase* ActivationSound;

	UPROPERTY(EditAnywhere)
	class USoundBase* ExplosionSound;

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMineMaterial;

	UFUNCTION()
    void BeginTankOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    void BeginBulletOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void BeginKillOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Explode();

	UFUNCTION()
	void Die();

	UFUNCTION()
	void Activate();
	
	
public:	
	// Sets default values for this actor's properties
	AMine();

	UFUNCTION()
    void Init(ATankPawn* SourcePawn);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

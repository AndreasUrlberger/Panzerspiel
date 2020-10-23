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
	UPROPERTY(VisibleAnywhere, Category="Mesh")
	class UStaticMeshComponent* MineMesh;

	UPROPERTY(VisibleAnywhere, Category="Mesh")
	class UStaticMeshComponent* ExplosionMesh;

	UPROPERTY(VisibleAnywhere, Category="Collision")
	class USphereComponent* TankTriggerSphere;

	UPROPERTY(VisibleAnywhere, Category="Collision")
	class USphereComponent* BulletTriggerSphere;

	UPROPERTY(VisibleAnywhere, Category="Collision")
	class USphereComponent* KillSphere;

	UPROPERTY(EditAnywhere, Category="Mine")
	float ExplosionRadius = 100;

	// ExplosionTime in seconds.
	UPROPERTY(EditAnywhere, Category="Mine")
	float ExplosionTime = 0.25;

	UPROPERTY(EditAnywhere, Category="Mine")
	float TimeAtMax = 0.1;

	UPROPERTY()
	float CurrentTimeAtMax;

	UPROPERTY()
	bool ExplosionAtMax;
	
	UPROPERTY()
	bool MineActive;

	UPROPERTY(EditAnywhere, Category="Mine")
	float BulletTriggerRadius = 25;

	UPROPERTY(EditAnywhere, Category="Mine")
	float TankTriggerRadius = 50;

	// Activation time in seconds.
	UPROPERTY(EditAnywhere, Category="Mine")
	float ActivationTime = 1;

	UPROPERTY()
	bool ExplosionRunning;

	// Is set true once Explode got called an wont be set to false at any time after that. With this bool we can make
	// sure that Explode will never be called two times.
	UPROPERTY()
	bool Triggered;

	UPROPERTY()
	float RunningTime = 0;

	UPROPERTY()
	class ATankPawn* TankPawn;

	UPROPERTY(EditAnywhere, Category="Sound")
	class USoundBase* ActivationSound;

	UPROPERTY(EditAnywhere, Category="Sound")
	class USoundBase* ExplosionSound;

	UPROPERTY()
	class UMaterialInstanceDynamic* DynamicMineMaterial;

	UFUNCTION()
    void BeginTankOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void BulletHitEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

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

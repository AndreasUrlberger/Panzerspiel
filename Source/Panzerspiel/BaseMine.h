// All rights reserved Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseMine.generated.h"

UCLASS()
class PANZERSPIEL_API ABaseMine : public AActor
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

	UPROPERTY(VisibleAnywhere)
	float RunningTime = 0;

	UPROPERTY(VisibleAnywhere)
	class ATankPawn* TankPawn;

	UPROPERTY(EditAnywhere)
	class USoundBase* ActivationSound;

	UPROPERTY(VisibleAnywhere)
	class AActor* OverlapActor;

	UFUNCTION()
    void BeginTriggerOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EndTriggerOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION()
	void BeginKillOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Explode();

	UFUNCTION()
	void Die();
	
	
public:	
	// Sets default values for this actor's properties
	ABaseMine();

	UFUNCTION()
    void Init(ATankPawn* SourcePawn);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

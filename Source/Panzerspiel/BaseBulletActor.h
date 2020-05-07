// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBulletActor.generated.h"

UCLASS()
class PANZERSPIEL_API ABaseBulletActor : public AActor
{
	GENERATED_BODY()
	
	// Variables
public:

private:

	UPROPERTY(VisibleDefaultsOnly)
		class UBoxComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly)
		class UStaticMeshComponent* BulletMesh;

	UPROPERTY(VisibleDefaultsOnly)
		class UProjectileMovementComponent* ProjectileComp;

	UPROPERTY(EditAnywhere)
		class USoundBase* WallHitSound;

	UPROPERTY(EditAnywhere)
		class USoundBase* BulletDestroySound;

	UPROPERTY(VisibleAnywhere)
		class ATankPawn* Source;

	UPROPERTY(EditAnywhere)
		int HitsBeforeDeath;
	// Methods
public:	
	// Sets default values for this actor's properties
	ABaseBulletActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void Init(class ATankPawn* Spawner);

private:
	UFUNCTION()
		void HitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
		void OverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void Die();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
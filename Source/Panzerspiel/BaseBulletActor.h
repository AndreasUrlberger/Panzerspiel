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
	UPROPERTY(VisibleDefaultsOnly)
		class UBoxComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly)
		class UStaticMeshComponent* BulletMesh;

	UPROPERTY(VisibleDefaultsOnly)
		class UProjectileMovementComponent* ProjectileComp;

	// Methods
public:	
	// Sets default values for this actor's properties
	ABaseBulletActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

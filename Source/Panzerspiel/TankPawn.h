// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TankPawn.generated.h"

UCLASS()
class PANZERSPIEL_API ATankPawn : public APawn
{
	GENERATED_BODY()

// Variables
private:
	//UPROPERTY(EditDefaultsOnly)
	UPROPERTY(VisibleAnywhere)
		class UStaticMeshComponent* PlayerMesh;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ABaseBulletActor> ToSpawnBullet;

	UPROPERTY(EditAnywhere)
		float BarrelLength;

	UPROPERTY(EditAnywhere)
		float MovementSpeed;

	UPROPERTY(EditAnywhere)
		float RotationSpeed;

	UPROPERTY(EditAnywhere)
		int8 MaxShots;

	UPROPERTY(VisibleAnywhere)
		int8 ActiveShots;

	UPROPERTY(VisibleAnywhere)
		float MoveForwardAxisValue;

	UPROPERTY(VisibleAnywhere)
		float MoveRightAxisValue;

	UPROPERTY(EditAnywhere)
		class USoundBase* FireSound;

// Methods
public:
	// Sets default values for this pawn's properties
	ATankPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void HitByBullet(ATankPawn* Enemy);
	UFUNCTION()
		void BulletDestroyed();

private:
	UFUNCTION()
	void Shoot();

	UFUNCTION()
	void MoveForward(float AxisValue);
	UFUNCTION()
	void MoveRight(float AxisValue);
	UFUNCTION()
		FVector GetBulletSpawnPoint();
	UFUNCTION()
		void Die();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

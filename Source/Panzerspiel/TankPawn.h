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
    // UPROPERTY(EditDefaultsOnly)
    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* BaseMesh;

    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* TurretMesh;

    // The bullet that will be spawned when the tank shoots.
    UPROPERTY(EditAnywhere, Category="Spawn")
    TSubclassOf<class ABaseBulletActor> ToSpawnBullet;

    UPROPERTY(EditAnywhere, Category="Spawn")
    TSubclassOf<class ABaseMine> ToSpawnMine;

    UPROPERTY(EditAnywhere, Category="Spawn")
    float BarrelLength;

    UPROPERTY(EditAnywhere, Category="Spawn")
    float BarrelHeight;

    UPROPERTY(EditAnywhere, Category="Movement")
    float MovementSpeed;

    // Rotation speed of the base not the tower.
    UPROPERTY(EditAnywhere, Category="Movement")
    float RotationSpeed;

    UPROPERTY(EditAnywhere, Category="Weapons")
    int8 MaxShots;
    
    UPROPERTY(VisibleAnywhere, Category="Weapons")
    int8 ActiveShots;

    UPROPERTY(EditAnywhere, Category="Weapons")
    int8 MaxMines;

    UPROPERTY(VisibleAnywhere, Category="Weapons")
    int8 ActiveMines;


    // Stores the axis value that the tank should move forward.
    UPROPERTY(VisibleAnywhere)
    float MoveForwardAxisValue;

    // Stores the axis value that the tank should turn right.
    UPROPERTY(VisibleAnywhere)
    float MoveRightAxisValue;

    UPROPERTY(EditAnywhere, Category="Weapons")
    class USoundBase* FireSound;

    UPROPERTY(EditAnywhere, Category="Weapons")
    class USoundBase* MinePlantSound;

    UPROPERTY(EditAnywhere)
    class USoundBase* ExplosionSound;

    // Methods
public:
    // Sets default values for this pawn's properties.
    ATankPawn();

    // Called every frame.
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input.
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Will be called by a Bullet when it hits this Tank.
    UFUNCTION()
    void Kill(ATankPawn* Enemy);
    // Will be called by a Bullet before it dies to inform this tank that it regains one shot.
    UFUNCTION()
    void BulletDestroyed();

    UFUNCTION()
    void MineDestroyed();

    UFUNCTION(BlueprintImplementableEvent, Category = "FX")
    void PlayNiagaraExplosion(FVector SpawnLocation);

    // Points the tanks tower towards the Target.
    UFUNCTION()
    void AlignTower(const FVector Target);

    UFUNCTION()
    bool MoveTo(FVector TargetLocation, float DeltaTime);
    
private:
    UFUNCTION()
    void Shoot();
    UFUNCTION()
    void PlaceMine();

    UFUNCTION()
    void MoveForward(float AxisValue);
    UFUNCTION()
    void MoveRight(float AxisValue);
    UFUNCTION()
    FVector GetBulletSpawnPoint() const;
    UFUNCTION()
    void Die();


protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
};

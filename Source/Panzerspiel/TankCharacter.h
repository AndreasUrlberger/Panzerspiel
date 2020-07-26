// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TankCharacter.generated.h"

UCLASS()
class PANZERSPIEL_API ATankCharacter : public ACharacter {
    GENERATED_BODY()

    // Variables
protected:
    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* BaseMesh;
    
    UPROPERTY(EditAnywhere, Category="Movement")
    float MovementSpeed = 450;
    
    // Rotation speed of the base not the tower.
    UPROPERTY(EditAnywhere, Category="Movement")
    float RotationSpeed = 2.25;

    // Stores the axis value that the tank should move forward.
    UPROPERTY(VisibleAnywhere)
    float MoveForwardAxisValue;

    // Stores the axis value that the tank should turn right.
    UPROPERTY(VisibleAnywhere)
    float MoveRightAxisValue;

private:
    

    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* TurretMesh;

    // The bullet that will be spawned when the tank shoots.
    UPROPERTY(EditAnywhere, Category="Spawn")
    TSubclassOf<class ABullet> ToSpawnBullet;

    UPROPERTY(EditAnywhere, Category="Spawn")
    TSubclassOf<class AMine> ToSpawnMine;

    UPROPERTY(EditAnywhere, Category="Spawn")
    float BarrelLength = 100;

    UPROPERTY(EditAnywhere, Category="Spawn")
    float BarrelHeight = 17;

    UPROPERTY(EditAnywhere, Category="Weapons")
    int8 MaxShots = 3;

    UPROPERTY(VisibleAnywhere, Category="Weapons")
    int8 ActiveShots;

    UPROPERTY(EditAnywhere, Category="Weapons")
    int8 MaxMines = 3;

    UPROPERTY(VisibleAnywhere, Category="Weapons")
    int8 ActiveMines;

    UPROPERTY(EditAnywhere, Category="Weapons")
    class USoundBase* FireSound;

    UPROPERTY(EditAnywhere, Category="Weapons")
    class USoundBase* MinePlantSound;

    UPROPERTY(EditAnywhere)
    class USoundBase* ExplosionSound;

    // Methods
public:
    // Sets default values for this character's properties
    ATankCharacter();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Will be called by a Bullet when it hits this Tank.
    UFUNCTION()
    void Kill(ATankCharacter* Enemy);
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

private:
    UFUNCTION()
    void Shoot();
    UFUNCTION()
    void PlaceMine();

    UFUNCTION()
    void MoveForward(float AxisValue);
    UFUNCTION()
    virtual void MoveRight(float AxisValue);
    UFUNCTION()
    FVector GetBulletSpawnPoint() const;
    UFUNCTION()
    void Die();


protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
};

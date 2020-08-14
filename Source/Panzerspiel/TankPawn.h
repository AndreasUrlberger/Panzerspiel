// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TankPawn.generated.h"

UCLASS()
class PANZERSPIEL_API ATankPawn : public APawn
{
	GENERATED_BODY()

// Variables
protected:
    UPROPERTY(VisibleAnywhere)
    class UStaticMeshComponent* BaseMesh;

    UPROPERTY(EditDefaultsOnly)
    class UFloatingPawnMovement *MovementComp;
    
    UPROPERTY(EditAnywhere, Category="Movement")
    float MovementSpeed = 450;
    
    // Rotation speed of the base not the tower.
    UPROPERTY(EditAnywhere, Category="Movement")
    float RotationSpeed = 135;

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

    UPROPERTY(EditAnywhere, Category="Movement")
    bool ControllerInput = false;

public:
    UPROPERTY(EditAnywhere, Category="Debug")
    bool DebugLog = false;

    // Methods
public:
    // Sets default values for this character's properties
    ATankPawn();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
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
    void ControllerMoveForward(float AxisValue);
    
    UFUNCTION()
    void ControllerMoveRight(float AxisValue);
    
    UFUNCTION()
    FVector GetBulletSpawnPoint() const;
    
    UFUNCTION()
    void Die();


protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // The Pawn is only moved via this function
    UFUNCTION()
    void MoveAndRotate(const FVector DeltaLocation, const FRotator DeltaRotation);

    // Checks if the tank is facing the movement direction and rotates it if necessary, due to this the tank might not move.
    UFUNCTION()
    void CalculateActualMovement(FVector TargetLocation, float DeltaTime);

    UFUNCTION()
    void ControllerMove();
};

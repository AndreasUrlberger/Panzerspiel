// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class PANZERSPIEL_API ABullet : public AActor
{
	GENERATED_BODY()
	
	// Variables
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
	class ATankCharacter* Source;

	// Tells whether the source tank can be killed by its own bullet.
	UPROPERTY(VisibleAnywhere)
	bool SourceVulnerable;

	// The "OtherActor" of the first OverlapBeginEvent when Source was null.
	UPROPERTY(VisibleAnywhere)
	AActor* FirstOverlapEventActor;

	UPROPERTY
	(EditAnywhere)
	int HitsBeforeDeath;
	// Methods
	public:
	// Sets default values for this actor's properties
	ABullet();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
    void Init(class ATankCharacter* Spawner);

	UFUNCTION()
    void Kill(ATankCharacter* Enemy);

	UFUNCTION(BlueprintImplementableEvent)
    void StopSmoke();

	UFUNCTION(BlueprintCallable)
    void FinalDie();

	private:
	UFUNCTION()
    void HitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                  FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
    void BeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    void EndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
    
	UFUNCTION()
    void Die();
	protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
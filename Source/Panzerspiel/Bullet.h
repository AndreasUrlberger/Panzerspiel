// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class PANZERSPIEL_API ABullet : public AActor
{
	GENERATED_BODY()
	
// Properties.
	private:

	UPROPERTY(VisibleDefaultsOnly)
	class UBoxComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly)
	class UStaticMeshComponent* BulletMesh;
	
	UPROPERTY(VisibleDefaultsOnly, Category="Movement")
	USceneComponent* RicochetPoint;

	UPROPERTY(EditAnywhere, Category="Sound")
	class USoundBase* WallHitSound;

	UPROPERTY(EditAnywhere, Category="Sound")
	class USoundBase* BulletDestroySound;

	UPROPERTY(VisibleInstanceOnly)
	class ATankPawn* Source;

	// Tells whether the source tank can be killed by its own bullet.
	UPROPERTY()
	bool SourceVulnerable = false;

	UPROPERTY(EditAnywhere, Category="Movement")
	int32 HitsBeforeDeath = 2;

	UPROPERTY()
	bool bIsDead = false;

	UPROPERTY(EditAnywhere, Category="Movement")
	float Speed = 1000;

	UPROPERTY(EditAnywhere, Category="Movement")
	float TraceHeight = 30;
	
// Functions.
	public:
	// Sets default values for this actor's properties
	ABullet();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
    void Init(class ATankPawn* Spawner);

	UFUNCTION()
    void Kill(ATankPawn* Enemy);

	UFUNCTION(BlueprintImplementableEvent)
    void StopSmoke();

	UFUNCTION(BlueprintCallable)
    void FinalDie();

private:
	
	UFUNCTION()
	void BulletMove(const float DeltaTime);

	UFUNCTION()
	// Returns true only if the hit tank actually dies and false if it is supposed to be ignored.
	bool TankHitEvent(ATankPawn* OtherTank);

	UFUNCTION()
	void BulletHitEvent(ABullet* OtherBullet);

	UFUNCTION()
	// Uses the Actors ForwardVector as bulletDirection and also updates it when ricocheting.
	void CalculateMove(const float DistanceToMove, const AActor* IgnoreActor);
    
	UFUNCTION()
    void Die();
	protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

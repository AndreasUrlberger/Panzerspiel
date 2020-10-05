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
	bool SourceVulnerable;

	// The "OtherActor" of the first OverlapBeginEvent when Source was null.
	UPROPERTY()
	AActor* FirstOverlapEventActor;

	UPROPERTY(EditAnywhere, Category="Movement")
	int32 HitsBeforeDeath = 2;

	UPROPERTY()
	bool bIsDead = false;

	UPROPERTY(EditDefaultsOnly, Category="Movement")
	USceneComponent* LeftCorner;
	
	UPROPERTY(EditDefaultsOnly, Category="Movement")
	USceneComponent* RightCorner;

	UPROPERTY(EditAnywhere, Category="Movement")
	float Speed = 1000;

	UPROPERTY(EditAnywhere, Category="Movement")
	float BulletLength = 20;

	UPROPERTY(EditAnywhere, Category="Movement")
	float BulletWidth = 15;

	UPROPERTY(EditAnywhere, Category="Movement")
	float DistanceTolerance = 1;

	UPROPERTY()
	bool bIsSliding = false;

	// Stores the direction the bullet will take after sliding.
	UPROPERTY()
	FVector NextDirection = FVector::ZeroVector;

	// Point till which the bullet is supposed to slide.
	UPROPERTY()
	FVector SlideEndPoint = FVector::ZeroVector;

	
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
    void HitEvent(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                  FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
    void BeginOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                      int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
    void EndOverlapEvent(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void BulletMove(const float DeltaTime);
    
	UFUNCTION()
    void Die();
	protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};

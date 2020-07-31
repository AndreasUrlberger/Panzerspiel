// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "SplineTestVisualizer.generated.h"

UCLASS()
class PANZERSPIEL_API ASplineTestVisualizer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineTestVisualizer();

	UPROPERTY(EditAnywhere, Category="Spline")
	class USplineComponent *SplineComp;

	UPROPERTY(EditAnywhere)
	bool AddPoint;

	UPROPERTY(EditAnywhere)
	class AAITankPawn *TankPawn;

	UPROPERTY(EditAnywhere)
	float MergeThreshold = 15;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoubleRicochetTest.generated.h"

UCLASS()
class PANZERSPIEL_API ADoubleRicochetTest : public AActor
{
	GENERATED_BODY()


private:
	UPROPERTY(EditAnywhere, Category="Debug")
	class ATankPawn *Tank1;

	UPROPERTY(EditAnywhere, Category="Debug")
	ATankPawn *Tank2;

	UPROPERTY(EditAnywhere, Category="Debug")
	class ACubeObstacle *Cube1;

	UPROPERTY(EditAnywhere, Category="Debug")
	ACubeObstacle *Cube2;

	UPROPERTY(EditAnywhere, Category="Debug")
	int32 Cube1EdgeIndex = 0;

	UPROPERTY(EditAnywhere, Category="Debug")
	int32 Cube2EdgeIndex = 0;

	UPROPERTY(EditAnywhere, Category="Debug")
	float LineThickness = 10;

	UPROPERTY(EditAnywhere, Category="Debug")
	float DisplayHeight = 0;

	UPROPERTY(EditAnywhere, Category="Debug")
	float FontScale = 10;
public:	
	// Sets default values for this actor's properties
	ADoubleRicochetTest();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	static float GetAngleBetween(FVector Vector1, FVector Vector2);

};

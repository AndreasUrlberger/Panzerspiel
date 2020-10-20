// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SecondDemo.generated.h"

UCLASS()
class PANZERSPIEL_API ASecondDemo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASecondDemo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

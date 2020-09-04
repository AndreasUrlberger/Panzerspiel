#pragma once
#include "CoreMinimal.h"
#include "BulletPath.generated.h"

USTRUCT()
struct FBulletPath {

	GENERATED_USTRUCT_BODY()
public:
	FBulletPath(FVector NewShootingDirection = FVector(0, 0, 0), float NewPathLength = 0)
    : Target(NewShootingDirection), PathLength(NewPathLength) { }

	UPROPERTY(VisibleAnywhere)
	FVector Target;

	UPROPERTY(VisibleAnywhere)
	float PathLength;
};
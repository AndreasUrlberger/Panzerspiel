#pragma once
#include "WorldObstacle.h"

class FUtility {
private:
	// Should never be created.
	FUtility();
public:
	// Arrays must be sorted for this function to work.
    static TArray<FObstacleEdge> IntersectArrays(const TArray<FObstacleEdge> &First, const TArray<FObstacleEdge> &Second);

	// Mirrors a given FVector2D (ToMirror) at a given Axis (MirrorDirection).
    static FVector2D MirrorVector(const FVector2D ToMirror, const FVector2D MirrorOrigin, const FVector2D MirrorDirection);

	// Mirrors a given Point (ToMirror) at a given Axis (MirrorDirection).
    static FVector2D MirrorPoint(const FVector2D ToMirror, const FVector2D MirrorOrigin, const FVector2D MirrorDirection);

	// Returns true if the Edge could reflect the bullet to the target according to their rotation.
	static bool CanBulletEverHitTarget(const FObstacleEdge& Edge, const FVector2D &BulletOrigin, const FVector2D &Target);

	// Only returns those edges which create, when considered as mirrors, a line of sight from the Origin to the Target.
	// Warning: This function makes use of up to three line traces and thus should only be used as the last option.
	static void FilterSingleRicochetLOS(const FObstacleEdge &Edge, const AActor *Origin, const AActor *Target,
		float RaycastHeight, float HitThreshold, TArray<struct FBulletPath> &BulletPaths);

	// Return true if there is no way a bullet reflected from one edge could ever hit the other edge.
	static bool AreFacingAway(const FObstacleEdge &Edge1, const FObstacleEdge &Edge2, const FVector2D &Edge1Normal);

	// Tells whether the bullet can, once reflected at the edge, hit the other edge, judging by direction and origin only.
	// This one is more precise (this one can exclude more edges) but also more expensive than AreFacingWay().
    static bool IsReflectionGonnaHit(const FObstacleEdge &ShooterEdge, const FObstacleEdge &TargetEdge,
    	const FVector2D &ShooterEdgeNormal, const FVector2D &ShooterLocation, const FVector2D &ShootDirection);

	// Calculates the intersection point between the two given lines.
	static FVector2D CalculateIntersect(const FVector2D &Edge1Start, const FVector2D &Edge1Dir, const FVector2D &Edge2Start, const FVector2D &Edge2Dir);

	// Only returns true if the edges, considered as mirrors, create a line of sight from the Origin to the Target.
	// Warning: This function makes use of up to three line traces and thus should only be used as the last option.
	static bool HasDoubleRicochetLOS(const FObstacleEdge& ShooterEdge, const FObstacleEdge& TargetEdge, const AActor *Shooter,
		const AActor *Target, const FVector2D &ShootDirection, const float RaycastHeight, const float DistanceThreshold, FBulletPath &BulletPath);
};

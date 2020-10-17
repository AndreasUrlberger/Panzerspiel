// All rights reserved @Apfelstrudel Games.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WorldObstacle.h"

#include "Utility.generated.h"

UCLASS()
class PANZERSPIEL_API UBroadLineTraceEdgeParams : public UObject{

	GENERATED_BODY()

	/*UBroadLineTraceEdgeParams(float NRadius, FVector2D NShotDir, UObstacleEdge* NTargetEdge, const AActor* NIgnoreActor,
		FVector2D NOriginLoc, float NTraceDistance, float NTraceHeight, float NDistanceThreshold) : Radius(NRadius),
		ShotDir(NShotDir), TargetEdge(NTargetEdge), IgnoreActor(NIgnoreActor), OriginLoc(NOriginLoc),
		TraceDistance(NTraceDistance), TraceHeight(NTraceHeight), DistanceThreshold(NDistanceThreshold){ }*/
public:
	UBroadLineTraceEdgeParams();

	UBroadLineTraceEdgeParams* Init(float NRadius, FVector2D NShotDir, UObstacleEdge* NTargetEdge, const AActor* NIgnoreActor,
        FVector2D NOriginLoc, float NTraceDistance, float NTraceHeight, float NDistanceThreshold);

	UPROPERTY()
	float Radius;

	UPROPERTY()
	FVector2D ShotDir;

	UPROPERTY()
	UObstacleEdge* TargetEdge;

	UPROPERTY()
	const AActor *IgnoreActor;

	UPROPERTY()
	FVector2D OriginLoc;

	UPROPERTY()
	float TraceDistance;

	UPROPERTY()
	float TraceHeight;

	UPROPERTY()
	float OnLineThreshold;
};

UCLASS()
class PANZERSPIEL_API UBroadLineTraceTargetParams : public UObject{

	GENERATED_BODY()

    /*UBroadLineTraceTargetParams(float NRadius, FVector2D& NShotDir, const AActor* NTarget, const AActor* NIgnoreActor,
        FVector2D& NOriginLoc, float NTraceDistance, float NTraceHeight, float NDistanceThreshold) : Radius(NRadius),
        ShotDir(NShotDir), Target(NTarget), IgnoreActor(NIgnoreActor), OriginLoc(NOriginLoc),
        TraceDistance(NTraceDistance), TraceHeight(NTraceHeight), DistanceThreshold(NDistanceThreshold){ }*/
public:
	UBroadLineTraceTargetParams();

	UBroadLineTraceTargetParams* Init(float NRadius, FVector2D NShotDir, const AActor* NTarget, const AActor* NIgnoreActor,
        FVector2D NOriginLoc, float NTraceDistance, float NTraceHeight);

	UPROPERTY()
	float Radius = 0;

	UPROPERTY()
	FVector2D ShotDir;

	UPROPERTY()
	const AActor* Target;

	UPROPERTY()
	const AActor *IgnoreActor;

	UPROPERTY()
	FVector2D OriginLoc;

	UPROPERTY()
	float TraceDistance;

	UPROPERTY()
	float TraceHeight;
};

/**
 * 
 */
UCLASS()
class PANZERSPIEL_API UUtility : public UObject
{
	GENERATED_BODY()

	static constexpr float LineStrength = 3.f;

	static constexpr bool IsPersistent = true;
	
	public:
	// Arrays must be sorted for this function to work.
    static TArray<UObstacleEdge*> IntersectArrays(const TArray<UObstacleEdge*> &First, const TArray<UObstacleEdge*> &Second);

	// Mirrors a given FVector2D (ToMirror) at a given Axis (MirrorDirection).
    static FVector2D MirrorVector(const FVector2D ToMirror, const FVector2D MirrorOrigin, const FVector2D MirrorDirection);

	// Mirrors a given Point (ToMirror) at a given Axis (MirrorDirection).
    static FVector2D MirrorPoint(const FVector2D ToMirror, const FVector2D MirrorOrigin, const FVector2D MirrorDirection);

	// Returns true if the Edge could reflect the bullet to the target according to their rotation.
	static bool CanBulletEverHitTarget(const UObstacleEdge* Edge, const FVector2D &BulletOrigin, const FVector2D &Target, UWorld* World = nullptr);

	// Only adds those edges to the BulletPaths which, when considered as mirrors, create a line of sight from the Origin
	// to the Target.
	// Warning: This function makes use of up to three line traces and thus should only be used as the last option.
	static void FilterSingleRicochetLOS(const UObstacleEdge* Edge, const AActor *Origin, const FVector& OriginLocation,
		const AActor *Target, float RaycastHeight, TArray<struct FBulletPath> &BulletPaths);

	// Same function as FilterSingleRicochetLOS but uses a width based trace which is performance heavier.
	static void FilterSingleRicochetLOSBroad(const UObstacleEdge* Edge, const AActor *Origin, const FVector& OriginLocation,
    const AActor *Target, const float TraceHeight, const float OnLineThreshold, const float Radius, TArray<FBulletPath> &BulletPaths);
	
	// Return true if there is no way a bullet reflected from one edge could ever hit the other edge.
	static bool AreFacingAway(const UObstacleEdge* Edge1, const UObstacleEdge* Edge2, const FVector2D &Edge1Normal);

	// Tells whether the bullet can, once reflected at the edge, hit the other edge, judging by direction and origin only.
	// This one is more precise (this one can exclude more edges) but also more expensive than AreFacingWay().
    static bool IsReflectionGonnaHit(const UObstacleEdge* ShooterEdge, const UObstacleEdge* TargetEdge,
    	const FVector2D &ShooterEdgeNormal, const FVector2D &ShooterLocation, const FVector2D &ShootDirection);

	// Calculates the intersection point between the two given lines.
	static FVector2D CalculateIntersect(const FVector2D &Edge1Start, const FVector2D &Edge1Dir, const FVector2D &Edge2Start, const FVector2D &Edge2Dir);

	// Draws debug lines for one frame to show where the BulletPaths go.
	static void ShowBulletPaths(const TArray<FBulletPath>& BulletPaths, const AActor *Origin, const float LineThickness = 15);

	// Draws debug lines for one frame to show each edge in the given array.
	static void ShowEdges(const TArray<const UObstacleEdge*> &EdgesToShow, const AActor *WorldReference, const float DisplayHeight = 205, const float LineThickness = 15);

	// Draws a debug line for one frame to show a single edge.
	static void DrawEdge(const UObstacleEdge* Edge, const FColor Color, const AActor *WorldReference, const float LineThickness = 15, const float DisplayHeight = 205);

	// Draws a debug line to one frame to show the given line.
    static void DrawLine(const FVector2D &Start, const FVector2D &End, const FColor Color, const AActor *WorldReference, const float LineThickness = 15, const float DisplayHeight = 205);

	// Checks whether the Target point can be reached from the From point.
	static bool HasLOSToTarget(FVector& From, const FVector& Target, const AActor* IgnoreActor, FHitResult& HitResult,
    float RaycastHeight, float DistanceThreshold);

	static bool IsPointOnLine(const FVector2D &Point, const UObstacleEdge* Edge, const float Threshold = 1.f);

	static float BroadLineTraceEdge(UBroadLineTraceEdgeParams* P);
	
	static float BroadLineTraceTarget(UBroadLineTraceTargetParams* P);

	// Only returns true if the edges, considered as mirrors, create a line of sight from the Origin to the Target.
	// Warning: -This function makes use of multiple line traces and thus should only be used as the last option.
	static bool HasDoubleRicochetLOS(const UObstacleEdge* ShooterEdge, const UObstacleEdge* TargetEdge, const AActor *Shooter, const FVector& ShooterLoc,
        const AActor *Target, const FVector2D &ShotDir, const float TraceHeight, FBulletPath &BulletPath);

	static FString PrintEdges(TArray<UObstacleEdge*> Edges);
};
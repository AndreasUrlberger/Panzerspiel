// Copyright Apfelstrudel Games - All Rights Reserved

#pragma once

#include "CoreMinimal.h"
//#include "../../../../../../../Program Files/Epic Games/UE_4.25/Engine/Plugins/Marketplace/UINavigation/Source/UINavigation/Public/UINavController.h"
#include "../Plugins/Marketplace/UINavigation/Source/UINavigation/Public/UINavController.h"
#include "TankPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PANZERSPIEL_API ATankPlayerController : public AUINavController
{
	GENERATED_BODY()

	ATankPlayerController();

private:
	UPROPERTY(EditAnywhere, Category = "Spawning")
    TSubclassOf<AActor> CrosshairToSpawn = nullptr;

	UPROPERTY(VisibleAnywhere)
	AActor* Crosshair = nullptr;

	UPROPERTY(EditAnywhere)
	float CrosshairHeight = 10;

	UPROPERTY(EditAnywhere, Category="Borders")
	float BorderTop = 0;

	UPROPERTY(EditAnywhere, Category="Borders")
	float BorderBottom = 0;

	UPROPERTY(EditAnywhere, Category="Borders")
	float BorderRight = 0;

	UPROPERTY(EditAnywhere, Category="Borders")
	float BorderLeft = 0;

	UPROPERTY(EditAnywhere, Category="Movement")
	bool ControllerInput = false;

	UPROPERTY()
	float CrosshairUpValue = 0;

	UPROPERTY()
	float CrosshairRightValue = 0;

	UPROPERTY(EditAnywhere, Category="Movement")
	float CrosshairMovementSpeed = 10;

public:

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetCrosshairVisibility(bool IsVisible);
	
	UFUNCTION(BlueprintCallable)
    void UseControllerInput(bool UsesController);
    

protected:
	virtual void SetupInputComponent() override;
	
private:

	UFUNCTION()
	void UpdateCrosshair() const;

	UFUNCTION()
	void ControllerUpdateCrosshair(float DeltaTime) const;

	UFUNCTION()
	void CrosshairMoveUp(float AxisValue);

	UFUNCTION()
	void CrosshairMoveRight(float AxisValue);
	
	UFUNCTION()
    void FireButtonPressed();
	
	UFUNCTION()
    void MineButtonPressed();

	UFUNCTION()
    void MoveForward(float AxisValue);
    
	UFUNCTION()
    virtual void MoveRight(float AxisValue);

	virtual void OnPossess(APawn* InPawn) override;
	
};

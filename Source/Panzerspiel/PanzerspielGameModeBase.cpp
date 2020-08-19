// Fill out your copyright notice in the Description page of Project Settings.


#include "PanzerspielGameModeBase.h"
#include "Kismet/GameplayStatics.h"

APanzerspielGameModeBase::APanzerspielGameModeBase() {
	if(DebugLog) UE_LOG(LogTemp, Warning, TEXT("Called APanzerspielGameModeBase"));
}

void APanzerspielGameModeBase::BeginPlay() {
	Super::BeginPlay();
	// TODO: Theres a third crosshair in the game when we have two players.
	/*if(PlayerControllers.Num() < MaxPlayers) {
		// Create New Player.
		APlayerController *Controller = UGameplayStatics::CreatePlayer(this);
		PlayerControllers.Init(Controller, 0);
	}*/
}
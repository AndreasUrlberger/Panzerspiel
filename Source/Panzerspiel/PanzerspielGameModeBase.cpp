// Fill out your copyright notice in the Description page of Project Settings.


#include "PanzerspielGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "TankPawn.h"

APanzerspielGameModeBase::APanzerspielGameModeBase() {
	if(DebugLog) UE_LOG(LogTemp, Warning, TEXT("Called APanzerspielGameModeBase"));
}

TArray<ATankPawn*> APanzerspielGameModeBase::GetTankPawns() const {
	return PlayerPawns;
}

TArray<ATankPawn*> APanzerspielGameModeBase::GetTankPawnsByTeam(int32 TeamIndex) {
	TArray<ATankPawn*> FilteredPawns;
	for(ATankPawn *Pawn: PlayerPawns) {
		if(Pawn->GetTeam() == TeamIndex)
		FilteredPawns.Add(Pawn);
	}
	return FilteredPawns;
}

TArray<ATankPawn*> APanzerspielGameModeBase::GetTankPawnsByTeams(TArray<int32> TeamIndices) {
	TArray<ATankPawn*> FilteredPawns;
	for(ATankPawn *Pawn: PlayerPawns) {
		if(TeamIndices.Contains(Pawn->GetTeam()))
			FilteredPawns.Add(Pawn);
	}
	return FilteredPawns;
}

TArray<ATankPawn*> APanzerspielGameModeBase::GetTankPawnNotInTeam(int32 TeamIndex) {
	TArray<ATankPawn*> FilteredPawns;
	for(ATankPawn *Pawn: PlayerPawns) {
		if(IsValid(Pawn) && Pawn->GetTeam() != TeamIndex)
			FilteredPawns.Add(Pawn);
	}
	return FilteredPawns;
}

void APanzerspielGameModeBase::TankPawnRemoveSelf(ATankPawn* Self) {
	PlayerPawns.Remove(Self);
}

void APanzerspielGameModeBase::TankPawnRegisterSelf(ATankPawn* Self) {
	PlayerPawns.Add(Self);
}

void APanzerspielGameModeBase::ClearTankPawns() {
	PlayerPawns.Empty();
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

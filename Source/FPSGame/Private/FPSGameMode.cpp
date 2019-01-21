// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();
}

void AFPSGameMode::BeginPlay() {

	Super::BeginPlay();

	SetCurrentState(EGamePlayState::EPlaying);
// getting player pawn
	MyCharacter = Cast<AFPSCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void AFPSGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (MyCharacter)
	{
		if (FMath::IsNearlyZero(MyCharacter->GetHealth(), 0.001f))
		{
			SetCurrentState(EGamePlayState::EGameOver);
		}
	}
}

EGamePlayState AFPSGameMode::GetCurrentState() const 
{
	return CurrentState;
}
 
void AFPSGameMode::SetCurrentState(EGamePlayState newState)
{
	CurrentState = newState;
	HandleNewState(CurrentState);
}

void AFPSGameMode::HandleNewState(EGamePlayState newState)
{
	switch (newState)
	{
		case EGamePlayState::EPlaying:
		{
			// do nothing
		}
		break;

		case EGamePlayState::EGameOver:
		{
			UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
		}
		break;

		case EGamePlayState::EUnknown:
		{
			// do nothing
		}
		break;
	}
}

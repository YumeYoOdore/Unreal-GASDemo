// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS_DemoGameMode.h"

AGAS_DemoGameMode::AGAS_DemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_ThirdPersonCharacter"));

	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

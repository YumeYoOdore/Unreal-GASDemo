// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.


#include "GAS_DemoAssetManager.h"
#include "AbilitySystemGlobals.h"

const FPrimaryAssetType UGAS_DemoAssetManager::WeaponItemType = TEXT("Weapon");
const FPrimaryAssetType UGAS_DemoAssetManager::ConsumableItemType = TEXT("Consumable");

UGAS_DemoAssetManager& UGAS_DemoAssetManager::Get()
{
	UGAS_DemoAssetManager* This = Cast<UGAS_DemoAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		return *NewObject<UGAS_DemoAssetManager>();
	}
}


void UGAS_DemoAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}


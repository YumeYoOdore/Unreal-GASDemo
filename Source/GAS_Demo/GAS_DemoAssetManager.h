// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "GAS_DemoAssetManager.generated.h"

/**
 * 
 */

class UItemBase;

UCLASS()
class GAS_DEMO_API UGAS_DemoAssetManager : public UAssetManager
{
	GENERATED_BODY()

/*
* Class GAS_DemoAssetManager
* AssetManager class used to define DataAsset types (used mainly for items
* and weapons)
*/

public:

	UGAS_DemoAssetManager() {}

	static const FPrimaryAssetType WeaponItemType;
	static const FPrimaryAssetType ConsumableItemType;

	virtual void StartInitialLoading() override;

	static UGAS_DemoAssetManager& Get();
	
};

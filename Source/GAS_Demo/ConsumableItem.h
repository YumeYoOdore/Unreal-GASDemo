// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "ConsumableItem.generated.h"

/**
 * 
 */
UCLASS()
class GAS_DEMO_API UConsumableItem : public UItemBase
{
	GENERATED_BODY()

/*
* Class UConsumableItem
* Child class of UItemBase, used to define a ConsumableItemType
*/

public:

	/** Constructor: Sets ItemType to ConsumableItemType : see UGAS_DemoAssetManager for more details on ItemTypes */
	UConsumableItem()
	{
		ItemType = UGAS_DemoAssetManager::ConsumableItemType;
	}
	
};

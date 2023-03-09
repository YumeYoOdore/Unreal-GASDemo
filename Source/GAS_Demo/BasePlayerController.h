// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GAS_DEMO_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()

/*
* Class ABasePlayerController
* Holds player's inventory while overriding the base PlayerController class
*/

public:
	ABasePlayerController() 
	: MaxInventoryCapacity(30),
		CurrentInventoryLoad(0)
	{}

	virtual void BeginPlay() override;

	//Inventory related functionality

	/** Function used to add items to inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool AddInventoryItem(UItemBase* Item, int32 ItemCount = 1);

	/** Function to used to remove an item from inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool RemoveInventoryItem(UItemBase* Item, int32 RemoveCount = 1);

	/** Function used to find a specific item from inventory based on name */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	UItemBase* FindItemInInventory(FName name);

	/** Function used to query item count from inventory based on name */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetInventoryItemAmount(FName name);

	/** Function used to retrieve inventory data as a whole */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	bool GetItemInInventoryCount(FName name, int32& LookupCount);

	/** Function used to retrieve current inventory load: this is the number
	* of unique items in inventory, not the total of items (which can be stackable)
	*/
	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetCurrentInventoryLoad();

	/** Utility function used to update CurrentInventoryLoad : not meant to be used outside code */
	void UpdateInventoryLoad();

private:

	/** Max inventory capacity */
	UPROPERTY(VisibleAnywhere, Category = Inventory)
	int32 MaxInventoryCapacity;

	/** How many UNIQUE items do we currently hold in inventory */
	UPROPERTY(VisibleAnywhere, Category = Inventory)
	int32 CurrentInventoryLoad;
	
	/** Map of currently held items in inventory */
	UPROPERTY(VisibleAnywhere, Category = Inventory)
	TMap<UItemBase*, int32> InventoryData;

public:

	/** Utility function to retrieve the Max Inventory Capacity which is used when picking up items */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	int32 GetMaxInventoryCapacity() const { return MaxInventoryCapacity; }

	/** Utility function to retrieve all items in inventory */
	UFUNCTION(BlueprintCallable, Category = Inventory)
	const TMap<UItemBase*, int32>& GetInventoryDataMap()
	{
		return InventoryData;
	}

};

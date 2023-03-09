// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.

#include "BasePlayerController.h"
#include "ItemBase.h"

bool ABasePlayerController::AddInventoryItem(UItemBase* Item, int32 ItemCount)
{
	/* Function AddInventoryIdem
	* Arguments: UItemBase Item - Item to add; int32 ItemCount - The quantity of items of type Item to add
	* Output: true if item was added successfully, false on failure
	*/

	if (!Item || ItemCount <= 0)
	{
		return false;
	}

	// Before including the item check if it is already in inventory
	bool bHasItem = (FindItemInInventory(Item->ItemName)) ? true : false;
	
	if (bHasItem)
	{
		//if we already have this item, increment count
		int32 LookupCount;
		GetItemInInventoryCount(Item->ItemName, LookupCount);

		InventoryData.Add(Item, LookupCount + ItemCount);
	}
	else
	{
		//Otherwise, directly add item to inventory
		InventoryData.Add(Item, ItemCount);
	}

	//call Update inventory load and return true for success
	UpdateInventoryLoad();
	return true;
}


bool ABasePlayerController::RemoveInventoryItem(UItemBase* Item, int32 RemoveCount)
{
	/* Function RemoveInventoryItem
	* Arguments: UItemBase Item - Item to remove; int32 ItemCount - The quantity of items of type Item to remove
	* Output: true if item was removed successfully, false on failure
	*/

	if (!Item || RemoveCount <= 0)
	{
		//Arguments are not valid, return false
		return false;
	}

	//Iterate through our InventoryData looking for the item to remove
	for (TPair<UItemBase*, int32>& Pair : InventoryData)
	{
		//If item is found then remove the quantity specified in RemoveCount
		if (Pair.Key->ItemName == Item->ItemName)
		{
			Pair.Value -= RemoveCount; //Once updated ItemCount, if it is 0 or less then remove it from the Inventory
			if (Pair.Value <= 0)
			{
				InventoryData.Remove(Pair.Key);
				UpdateInventoryLoad();
			}
			return true;
		}
	}

	// Update Inventory load and return false for failure (we only reach this point if we didn't enter the condition above)
	UpdateInventoryLoad();
	return false;
}


UItemBase* ABasePlayerController::FindItemInInventory(FName name)
{
	/* Function FindItemInInventory
	* Arguments: FName name - target's Item name value
	* Output: A pointer to the item in inventory
	*/

	for (TPair<UItemBase*, int32>& Pair : InventoryData)
	{
		if (Pair.Key->ItemName == name)
		{
			return Pair.Key;
		}
	}
	return nullptr;
}

int32 ABasePlayerController::GetInventoryItemAmount(FName name)
{
	/* Function GetInventoryItemAmount
	* Arguments: FName name - target's Item name value
	* Output: The count value of the target item 
	*/

	for (TPair<UItemBase*, int32>& Pair : InventoryData)
	{
		if (Pair.Key->ItemName == name)
		{
			return Pair.Value;
		}
	}
	//Since we cannot have items with amount less than 1 we return 0
	//if item is not found
	return 0;
}




bool ABasePlayerController::GetItemInInventoryCount(FName name, int32& LookupCount)
{
	/* Function GetItemInInventoryCount
	* Arguments: FName name - target's Item name value, int32& LookupCount - address of int32 where itemCount is stored 
	* Output: True if successfully found the item and retrieved the item count, false on failure
	*/

	for (TPair<UItemBase*, int32>& Pair : InventoryData)
	{
		if (Pair.Key->ItemName == name)
		{
			LookupCount = Pair.Value;
			return true;
		}
	}
	LookupCount = 0;
	return false;
}



int32 ABasePlayerController::GetCurrentInventoryLoad()
{
	/* Function GetCurrentInventoryLoad
	* Arguments: none
	* Output: CurrentInventoryLoad value
	*/

	//Update inventory load before query
	UpdateInventoryLoad();
	return CurrentInventoryLoad;
}


void ABasePlayerController::UpdateInventoryLoad()
{
	/* Function UpdateInventoryLoad
	* Arguments: none
	* Output: none (Update's InventoryLoad value: useful on any inventory-related operation
	*/

	CurrentInventoryLoad = InventoryData.Num();
}


void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
}
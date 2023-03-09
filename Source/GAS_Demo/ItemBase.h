// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Styling/SlateBrush.h"
#include "GAS_DemoAssetManager.h"
#include "ItemBase.generated.h"

class UGASGameplayAbility;
class UGameplayAbility;

UCLASS(Abstract, BlueprintType)
class GAS_DEMO_API UItemBase : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
/*
* Class UItemBase
* Abstract class to be used as a Parent of all Items in the project
*/

public:
	UItemBase() {};

	/** Definition of item Type */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
	FPrimaryAssetType ItemType;

	/** Item name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FName ItemName;

	/** Item Description */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FString ItemDescription;

	/** ID number of item : can be useful if there will be a lot of items in the project and need
	* to keep track of them eventually, could also help making a database out of all items
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	int32 ItemId;

	/** Definition of item display icon: should be used on Inventory UI */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
	FSlateBrush ItemIcon;

	/** Definition of item's GrantedAbility */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TSubclassOf<UGameplayAbility> GrantedAbility;
};

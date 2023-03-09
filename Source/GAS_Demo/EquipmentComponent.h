// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h" 
#include "GameplayAbilitySpec.h"
#include "BasePlayerController.h"
#include "EquipmentComponent.generated.h"

class UWeaponBase;

UCLASS( Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GAS_DEMO_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

/*
* Class EquipmentComponent
* ActorComponent class intended to provide a layer of modularity to the project
* this component manages anything equipment-related and communicates to the
* CharacterBase any Equipment changes made
*/

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	* Used to equip a weapon from the inventory into the character
	* returns true if weapon was successfully equipped, false on failure
	* 
	* @param WeaponToEquip  Reference to the weapon from the inventory to equip
	* @param Owner  Character owner of the equipment/Inventory
	*/
	UFUNCTION(BlueprintCallable, Category = ChangeEquipment)
	bool EquipWeapon(UWeaponBase* WeaponToEquip);
	
	/**
	* Used to remove a equipped weapon from the character
	* @param WeaponToEquip  Reference to the weapon to unequip
	*/
	UFUNCTION(BlueprintCallable, Category = ChangeEquipment)
	void RemoveWeapon();

	/**
	* Used to equip a Consumable item for the character's use
	* @param ItemToEquip Reference to the Item to equip
	*/
	UFUNCTION(BlueprintCallable, Category = ChangeEquipment)
	bool EquipConsumable(UItemBase* ItemToEquip);

	/**
	* Used to remove an equipped Consumable from the character
	* @param Item Reference to the Item to remove
	*/
	UFUNCTION(BlueprintCallable, Category = ChangeEquipment)
	void RemoveConsumable(UItemBase* Item);

	/** Used to cycle on the slottedItems to equip the next item in slot list */
	UFUNCTION(BlueprintCallable, Category = SlottedItems)
	void EquipNextItem();

	/** Used to cycle on the slottedWeapons to equip the next weapon in slot list */
	UFUNCTION(BlueprintCallable, Category = SlottedItems)
	void EquipNextWeapon();

	//Pure utility function will remove the EquippedConsumableItem
	void UnequipConsumable() { EquippedConsumableItem = nullptr; }

	/*
	* Use the following functions to Slot Weapons/Items to be equipped
	*/
	
	UFUNCTION(BlueprintCallable, Category = SlottedItems)
	void SlotWeapon(UWeaponBase* Weapon);

	UFUNCTION(BlueprintCallable, Category = SlottedItems)
	void SlotItem(UItemBase* Item);

public:
	/** Reference to the currently equipped weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapons)
	UWeaponBase* EquippedWeaponItem;

	/** Reference to the currently equipped consumable item */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapons)
	UItemBase* EquippedConsumableItem;

	/** Reference to the currently equipped weapon's GameplayAbilitySpecHandle (returned when equipping weapon) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Weapons)
	FGameplayAbilitySpecHandle AttackAbilitySpecHandle;


protected:
	/*
	* Following up on the Souls-like design, we slot 0 to n items
	* and cycle-equip them by pressing arrow keys.
	* For this Demo we're auto-slotting items once we pick them up
	*/

	/** Array of currently Slotted weapons, ready to be equipped */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SlottedItems)
	TArray<UWeaponBase*> SlottedWeapons;

	/** Array of currently Slotted consumables, ready to be equipped */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SlottedItems)
	TArray<UItemBase*> SlottedConsumables;

	/** Reference to the currently equipped item's GameplayAbilitySpecHandle (returned when equipping item) */
	FGameplayAbilitySpecHandle EquippedConsumableSpecHandle;

	/*
	* Utility Variables to save the indexes of currently equipped items
	* these will help swapping equipment straightforward
	*/
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = SlottedItems)
	int EquippedWeaponIndex;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = SlottedItems)
	int EquippedConsumableIndex;

	//Getting a reference of player controller for communication with inventory
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SlottedItems)
	ABasePlayerController* PlayerControllerRef;

};

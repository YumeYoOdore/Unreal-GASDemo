// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.


#include "EquipmentComponent.h"
#include "CharacterBase.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	EquippedWeaponItem = nullptr;

	EquippedWeaponIndex = 0;
	EquippedConsumableIndex = 0;
	
	//Adding a nullptr at the beginning of the SlottedWeapons array to make sure we have the bare-handed
	//combat type at the beginning of the array
	SlottedWeapons.Add(nullptr);
	// ...
}


// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerControllerRef = Cast<ABasePlayerController>(GetOwner()->GetInstigatorController());
	// ...
	
}


// Called every frame
void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


bool UEquipmentComponent::EquipWeapon(UWeaponBase* WeaponToEquip)
{
	/* Function EquipWeapon
	* Arguments: UWeaponBase WeaponToEquip - Pointer to the Weapon to Equip
	* Output: true if Weapon was successfully equipped, false on failure
	*/

	if (!WeaponToEquip) return false;

	if (EquippedWeaponItem)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Already have a weapon equipped, exiting"));
		return false;
	}

	ACharacterBase* MyChar = Cast<ACharacterBase>(GetOwner());

	if (!MyChar) return false;

	UAbilitySystemComponent* MyAbilitySystemComp = MyChar->GetAbilitySystemComponent();
	EquippedWeaponItem = WeaponToEquip;

	// Make sure we could correctly retrieve ability system component
	// and weapon to equip has a GrantedAbility, then give ability
	if (MyAbilitySystemComp && WeaponToEquip->GrantedAbility)
	{
		AttackAbilitySpecHandle = MyAbilitySystemComp->GiveAbility(WeaponToEquip->GrantedAbility);
	}

	return true;
}


void UEquipmentComponent::RemoveWeapon()
{
	/* Function RemoveWeapon
	* Arguments: none
	* Output: none (If a Weapon is equipped removes it)
	*/

	if (!EquippedWeaponItem)
	{
		//No equipped weapon, returning
		return;
	}

	ACharacterBase* MyChar = Cast<ACharacterBase>(GetOwner());

	if (!MyChar) return;

	//Remove granted ability
	UAbilitySystemComponent* MyAbilitySystemComp = MyChar->GetAbilitySystemComponent();
	if (MyAbilitySystemComp) {
		MyAbilitySystemComp->ClearAbility(AttackAbilitySpecHandle);
		EquippedWeaponItem = nullptr;
	}
}

bool UEquipmentComponent::EquipConsumable(UItemBase* ItemToEquip)
{
	/* Function EquipConsumable
	* Arguments: UItemBase ItemToEquip - Pointer to item to be equipped
	* Output: true if item was successfully equipped, false on failure
	*/

	ACharacterBase* MyOwner = Cast<ACharacterBase>(GetOwner());

	//Failed to Cast Owner to ACharacterBase
	if (!MyOwner) return false;

	//Running IsValid to make sure we DO have an item to equip and isn't pending removal or garbage collection
	//if it is valid we equip item, otherwise return false for non-success

	if (IsValid(ItemToEquip))
	{
		if (ItemToEquip->GrantedAbility && MyOwner->GetAbilitySystemComponent()) 
		{
			//Before giving new abilities make sure we remove previous granted abilities
			if (EquippedConsumableSpecHandle.IsValid())
			{
				MyOwner->GetAbilitySystemComponent()->ClearAbility(EquippedConsumableSpecHandle);
			}

			//Grant Item's Ability to the player
			FGameplayAbilitySpec GrantedAbilitySpec = FGameplayAbilitySpec(ItemToEquip->GrantedAbility.GetDefaultObject(), 1, 0);
			EquippedConsumableSpecHandle = MyOwner->GetAbilitySystemComponent()->GiveAbility(GrantedAbilitySpec);
		}

		EquippedConsumableItem = ItemToEquip;

		return true;
	}
	else
	{
		UnequipConsumable();
		return false;
	}
}

void UEquipmentComponent::RemoveConsumable(UItemBase* Item)
{
	/* Function RemoveConsumable
	* Arguments: UItemBase Item - Pointer to item to be removed
	* Output: none (Removes 1 Unity of Item from inventory when calling this function
	* if doing this makes the specified Item Quantity become 0 remove the Item entirely
	* from inventory and from SlottedConsumables)
	*/
	
	//Make sure Item is Valud
	if (!Item) return;

	//Make sure controller is valid
	if (!PlayerControllerRef) return;

	int32 currentItemAmount = PlayerControllerRef->GetInventoryItemAmount(Item->ItemName);

	// Item found
	if (currentItemAmount > 0)
	{
		//If removing 1 item makes it become 0 below then remove from Slotted AND from inventory
		//Update Inventory
		PlayerControllerRef->RemoveInventoryItem(Item, 1);

		if (currentItemAmount - 1 <= 0) 
		{
			UnequipConsumable();
			//Update SlottedConsumables
			SlottedConsumables.Remove(Item);
		}
	}
}

void UEquipmentComponent::EquipNextItem() 
{
	/* Function EquipNextItem
	* Arguments: none
	* Output: none (loops through SlottedConsumables and Equips the next Item in slot
	* if no Item is available in such slot, Unequip consumable)
	* 
	* This function takes direct inspiration from Dark Souls, where you can "Equip" more than one item
	* and cycle through them readying to be used (in this case this is what I consider Slot; probably
	* it is the other way around... but for consistency sake I'm calling "Slotting" to add more than
	* one item ready to be taken for player's use, and equipping to actually taking it).
	* 
	* if a certain slot has an item equipped but gets depleted, instead of "pushing" the items back and forth
	* the slot remains empty and going through that slot will result on no item being equipped;
	* Same goes if no item is added to a slot between other 2 items as follows:
	* O X O X X     <== O Item Available, X No item Available; running this function in this example
	* will result on the player Equipping item on index 0 -> unequipping items -> equipping item on index 2
	* and unequipping items when cycling through slots on index 3 and 4
	* 
	*/

	//If SlottedConsumables has no elements exit
	if (SlottedConsumables.Num() <= 0)  return;

	//Before requesting to equip the next item make sure we are not going beyond the max SlottedItemArray index
	//in that case, cycle back to the first element
	EquippedConsumableIndex = EquippedConsumableIndex + 1 < SlottedConsumables.Num() ? EquippedConsumableIndex + 1 : 0;
	
	EquipConsumable(SlottedConsumables[EquippedConsumableIndex]);
}

void UEquipmentComponent::EquipNextWeapon() 
{
	/* Function EquipNextWeapon
	* Arguments: none
	* Output: none (loops through SlottedWeapons and Equips the next Weapon in slot
	* if no Weapon is available in such slot, Unequip Weapons)
	* 
	* See the EquipNextItem for further details on this function's design
	*/

	//Before requesting to equip the next item make sure we are not going beyond the max SlottedItemArray index
	//in that case, cycle back to the first element
	EquippedWeaponIndex = EquippedWeaponIndex + 1 < SlottedWeapons.Num() ? EquippedWeaponIndex + 1 : 0;
	UWeaponBase* WeaponToEquip = SlottedWeapons[EquippedWeaponIndex];

	ACharacterBase* MyOwner = Cast<ACharacterBase>(GetOwner());
	if (MyOwner)
	{
		MyOwner->OnEquipmentChanged(EquippedWeaponItem, EEquipmentChangeStatus::Unequip);
	}

	RemoveWeapon();

	//If SlottedWeapon = nullptr means we're fighting "bare-handed", so will call UnequipWeapon function
	if (WeaponToEquip)
	{
		EquipWeapon(WeaponToEquip);
	}
}

void UEquipmentComponent::SlotWeapon(UWeaponBase* Weapon)
{
	/* Function SlotWeapon
	* Arguments: UWeaponBase Weapon - Pointer to a weapon to be added to the slot
	* Output: none (use for slotting a weapon into SlottedWeapons)
	*
	* Usually this would be handled on inventory/UI: in this Demo for simplicity sake we will slot
	* any Items we pickup directly
	*/

	//Make sure we don't slot the same weapon more than once
	if (SlottedWeapons.Find(Weapon) == INDEX_NONE)
		SlottedWeapons.Add(Weapon);
}

void UEquipmentComponent::SlotItem(UItemBase* Item)
{
	/* Function SlotItem
	* Arguments: UItemBase Item - Pointer to a weapon to be added to the slot
	* Output: none (use for slotting an item into SlottedConsumables)
	*/

	//Just make sure we don't slot the same item more than once
	if (SlottedConsumables.Find(Item) == INDEX_NONE)
	{
		SlottedConsumables.Add(Item);
	}
}
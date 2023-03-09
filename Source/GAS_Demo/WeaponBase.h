// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.

#pragma once

#include "CoreMinimal.h"
#include "ItemBase.h"
#include "AttributeSet.h"
#include "DataTypes.h"
#include "WeaponBase.generated.h"

/**
 * 
 */

UCLASS()
class GAS_DEMO_API UWeaponBase : public UItemBase
{
	GENERATED_BODY()

/*
* Class UWeaponBase
* Child class of UItemBase, used to define WeaponItemType
* Weapons unlike regular items should also have attribute values
* to grant to the player later on.
*/

public:
	//Base constructor, handling it here rather than on CPP as we're only defaulting all values on 0
	UWeaponBase()
	{
		ItemType = UGAS_DemoAssetManager::WeaponItemType;
		Damage.MinDamage = 0.f;
		Damage.MaxDamage = 0.f;
		
		Health = 0.f;
		Stamina = 0.f;
		Mana = 0.f;
		AttackPower = 0.f;
		Defense = 0.f;
		Strength = 0.f;
		Dexterity = 0.f;
		Vitality = 0.f;
		Endurance = 0.f;
		Intelligence = 0.f;
		Mind = 0.f;
		Agility = 0.f;
		CriticalRate = 0.f;
	}

	/** Weapon Actor to be spawned when equipping weapon */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
	TSubclassOf<AActor> WeaponActor;

	//Weapon Base Damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
	FWeaponCapturedDamage Damage;
	
	//Attributes to capture on weapon : These are applied to the character attribute set on equipped

	//Health: Modifier that directly increases Player's Max Health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
	float Health;

	//Stamina: Modifier that directly increases Player's Max Stamina
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
	float Stamina;

	//Mana: Modifier that directly increases Player's Max Mana
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
	float Mana;

	//AttackPower: Raw attack power that directly increases Player's damage output
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
	float AttackPower;

	//Defense: Raw defense that directly increases Player's damage absorbtion
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
	float Defense;

	//Strength: STR Stat that is later applied into AttackPower (WoW Style: 1 STR = 2 AttkPow)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
	float Strength;

	//Dexterity: DEX Stat that is later applied into AttackPower (WoW Style: 1 DEX = 2 AttkPow) : Also modifies CriticalRate
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
	float Dexterity;

	//Vitality: VIT Stat that is later applied into Defense And Max Health
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
	float Vitality;

	//Endurance: END Stat that is later applied into Max Stamina
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
	float Endurance;

	//Intelligence: INT Stat that is later applied into AttackPower (Used for magic only, exception is for weapons that scale with INT where 1 INT = 2 AttkPow)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
	float Intelligence;

	//Mind: MND Stat that is later applied into AttackPower (Used for Miracles only, exception is for weapons that scale with MND where 1 MND = 2 AttkPow)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
	float Mind;

	//Agility: AGI Stat that is later applied into AttackPower (Used for Ranged Weapons only, 1 AGI = 2 AttkPow)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
	float Agility;

	//CriticalRate: Raw critical rate in percentage, modifies rate in which player will trigger a Crittical hit (Design Info: don't increase this more than 10%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Attributes)
	float CriticalRate;
};

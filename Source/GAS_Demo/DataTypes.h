#pragma once

#include "UObject/PrimaryAssetId.h"
#include "DataTypes.generated.h"

class UItemBase;


UENUM()
enum EEquipmentChangeStatus
{
	Equip   UMETA(DisplayName = "Equip"),
	Unequip   UMETA(DisplayName = "UnEquip")
};

/** Struct used for defining weapon damages, these define a min and max range from which base attack is calculated */
USTRUCT(BlueprintType)
struct FWeaponCapturedDamage
{
	GENERATED_BODY()


public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		float MinDamage;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		float MaxDamage;
};

/** DataTypes.h - Utility Header class that defines DataTypes that are used by more than one script, and may even need to be called in Blueprint. */
// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.
// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GASAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class GAS_DEMO_API UGASAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

/*
* Class UGASAttributeSet
* Defines our project's Attribute set, also overrides the base functions
* required to handle calculations related directly to attribute changes
* 
* Important note: This project does not use any networking functionality
* therefore it doesn't include the relevant networking functions that can
* be used when defining attribute data: most tutorials online do include
* these lines regardless of project using or not using them, this only shows
* that they are not required and can be omitted if your project doesn't
* require them.
*/

public:

	UGASAttributeSet()
	{}

	void AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty);

	//Overriden functions
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	/*
	* The following lines define the base attributes used by our AbilitySystem
	* Important design note:
	* DO NOT DEFINE MORE ATTRIBUTES THAN THE ONES YOU'RE GOING TO USE ON YOUR PROJECT
	* this is usually handled directly on the game design phase through a GDD, but
	* for some rookie developers this is very important as sometimes small project are
	* not even documented before starting to work on them.
	*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxMana;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Mana;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Stamina;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxStamina;

	//Attack output power modifier
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData AttackPower;

	//Attributes used to capture equipped weapon Min and Max damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MinWeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxWeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Defense;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Strength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Dexterity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Vitality;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Agility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData CriticalRate;


	//The following stats will be unused for the current project
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Endurance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Intelligence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Mind;


public:

	//The following portion uses the ATTRIBUTE_ACCESSORS macro defined in the beginning of this file
	// this should define a way to GET, SET, and INIT the property and values.

	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Mana)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, MaxMana)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Stamina)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, MaxStamina)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, AttackPower)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, MinWeaponDamage)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, MaxWeaponDamage)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Defense)

	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Strength)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Dexterity)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Vitality)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Endurance)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Intelligence)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Mind)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, Agility)
	ATTRIBUTE_ACCESSORS(UGASAttributeSet, CriticalRate)

};

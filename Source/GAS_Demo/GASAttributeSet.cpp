// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.


#include "GASAttributeSet.h" 
#include "CharacterBase.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"


/* 
* Taken from ARPG Demo : This is used when initializing attributes to equalize the current attribute value with the max attribute value
* as on the curvetable only the maxValue is being defined.
* 
* Could probably go with a more specific and even elegant solution, but this seems to work just fine so I'm just recycling it here
* as with other portions of borrowed code studied the functionality before implementing it here
*/
void UGASAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

/*
* This function is called before any change to an attribute is made 
* in this project as well as with Epic's ARPG demo is most notably used
* to equalize Current values with Max Values of attributes at the beginning
* as the CurveTable only contains Max Values
*/
void UGASAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute()) 
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		AdjustAttributeForMaxChange(Stamina, MaxStamina, NewValue, GetStaminaAttribute());
	}
}

/*
* This function is called just before a GameplayEffect is executed and modifies an attribute's value
* it is most notably used to handle any final calculations before affecting any attributes. Also this function here
* calls the CharacterBase HandleDamage and HandleHealthChanged functions which will pass in the FINAL attribute
* attribute change values onto the blueprintable functions for UI handlings
*/
void UGASAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	/*
	* Function PostGameplayEffectExecute
	* Input: FGameplayEffectModCallbackData Data: data structure that contains all relevant info of the GameplayEffectModifier
	* Output: none (handle any calculations just before executing the GameplayEffect that will affect any Attribute)
	*/

	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();

	AActor* SourceActor = nullptr; //Damage instigator
	AController* SourceController = nullptr; //Controller of the damage instigator
	ACharacterBase* TargetCharacter = nullptr; //Damage target
	ACharacterBase* SourceCharacter = nullptr; //Damage instigator character
	float Magnitude = 0.f; //float pointer to the damage dealt

	FHitResult HitResult;

	//first retrieve the damage 
	if (Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Additive || Data.EvaluatedData.ModifierOp == EGameplayModOp::Type::Override)
	{
		Magnitude = Data.EvaluatedData.Magnitude;
	}

	//Retrieve hit result for post damage effects handling
	if (Context.GetHitResult())
	{
		HitResult = *Context.GetHitResult();
	}

	//Run this only if we are affecting Health Attribute
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		if (Source)
		{
			SourceActor = Source->GetAvatarActor();
			TargetCharacter = Cast<ACharacterBase>(Data.Target.AbilityActorInfo->AvatarActor.Get());

			if (SourceActor)
			{
				if (APawn* Pawn = Cast<APawn>(SourceActor))
				{
					SourceController = Pawn->GetController();
				}
			}

			if (SourceController)
			{
				SourceCharacter = Cast<ACharacterBase>(SourceController->GetPawn());
			}

			if (TargetCharacter)
			{
				//Clamp health value before any changes to ensure we don't go below 0 or Above MaxHealth
				SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));

				//Send Health change Magnitude to be used on Blueprints
				TargetCharacter->HandleHealthChanged(FMath::Abs(Magnitude));
				//Send all relevant Damage information to be used on blueprints
				//Important note for those new to GAS: The actual damage has already been handled into the attributes
				//this is ONLY for any additional handlings such as displaying damage numbers or updating UI health bar
				TargetCharacter->HandleDamage(Magnitude, HitResult, SourceCharacter, SourceActor);
			}
		}
	}
	//Run this only if we are affecting Mana Attribute
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		//Clamp mana value before any changes to ensure we don't go below 0 or Above MaxMana
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
		
		if (TargetCharacter) 
		{
			//Send Mana change Magnitude to be used on Blueprints
			TargetCharacter->HandleManaChanged(FMath::Abs(Magnitude));
		}
	}
}

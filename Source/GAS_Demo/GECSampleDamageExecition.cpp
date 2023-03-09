// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.


#include "GECSampleDamageExecition.h"
#include "GASAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CharacterBase.h"

/*
* Before proceeding any further, there is a lot of templates and boilerplate code used in this class
* it may seem very confusing at first but it is actually really straightforward
*/

// Step 1: Define Attributes to be affected inside a struct to access them below:
struct FMeleeDamageStatics
{
	//These macros can be picked up from GameplayEffectExecutionCalculation.h
	DECLARE_ATTRIBUTE_CAPTUREDEF(Health);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Mana);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Strength);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MinWeaponDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxWeaponDamage);

	FMeleeDamageStatics()
	{
		//Arguments for this macro are
		// S: StaticClass or Attribute class to be used
		// P: Property or Attribute to be captured
		// T: value of enum class EGameplayEffectAttributeCaptureSource defined in GameplayEffectTypes.h (Source or Target)
		// B: snapshot value? (bool) - Checks if the attribute is captured at the moment of calling this function or not
		// if true then it will use the "original" value before any change; if false it will re-calculate the value to the latest
		// value it has (useful when any change happens right before applying damage)
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, Health, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, Mana, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, Defense, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, AttackPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, Strength, Source, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, MinWeaponDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, MaxWeaponDamage, Source, false);

	}
};

static const FMeleeDamageStatics& MeleeDamageStatics()
{
	static FMeleeDamageStatics DmgStatics;
	return DmgStatics;
}

//2. On our constructor, tell is which attributes are relevant and should be captured
UGECSampleDamageExecition::UGECSampleDamageExecition()
{
	//This is a TArray defined in the base class GameplayEffectCalculation.h
	RelevantAttributesToCapture.Add(MeleeDamageStatics().HealthDef);
	RelevantAttributesToCapture.Add(MeleeDamageStatics().ManaDef);
	RelevantAttributesToCapture.Add(MeleeDamageStatics().DefenseDef);
	RelevantAttributesToCapture.Add(MeleeDamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(MeleeDamageStatics().StrengthDef);
	RelevantAttributesToCapture.Add(MeleeDamageStatics().MinWeaponDamageDef);
	RelevantAttributesToCapture.Add(MeleeDamageStatics().MaxWeaponDamageDef);
}

//3. Override the Execute_Implementation function from the base class
void UGECSampleDamageExecition::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//4. FGameplayEffectCustomExecutionParameters is a data structure that will hold all the relevant AbilitySystemsComponents data
	//create variables to capture the AbilitySystemComponents from Target and Source : this is basically boilerplate code
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();

	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;
	AActor* TargetActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;

	//5. Get FGameplayEffectSpecs from the Execution params struct
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	//6. Gather the tags from the source and target as that can further affect the calculation
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	//This is required for capturing the actual attributes
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	int32 attackerLevel = 0;
	
	//For our own calculation: getting attacker level
	if (Cast<ACharacterBase>(SourceAbilitySystemComponent->GetOwnerActor()))
		attackerLevel = Cast<ACharacterBase>(SourceAbilitySystemComponent->GetOwnerActor())->GetCharacterLevel();

	//For our own calculation: defining variables that will be used on calculations
	float AttackPower = 0.f;
	float Defense = 0.f;

	float Strength = 0.f;

	float MinDamage = 0.f;
	float MaxDamage = 0.f;

	//Setting damage at 5-10 as base unarmed damage
	float BaseMinDamage = 5.f;
	float BaseMaxDamage = 10.f;

	//Offensive attributes capture (AttemptCalculateCapturedAttributeMagnitude retrieves the value captuired in attribute set)
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MeleeDamageStatics().AttackPowerDef, EvaluationParameters, AttackPower);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MeleeDamageStatics().MinWeaponDamageDef, EvaluationParameters, MinDamage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MeleeDamageStatics().MaxWeaponDamageDef, EvaluationParameters, MaxDamage);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MeleeDamageStatics().StrengthDef, EvaluationParameters, Strength);

	//Defensive attributes capture
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MeleeDamageStatics().DefenseDef, EvaluationParameters, Defense);

	//Make sure attack power, VIT, DEF, STR don't go below 0
	if (AttackPower < 0.f)
		AttackPower = 0.f;
	if (Defense < 0.f)
		Defense = 0.f;
	if (Strength < 0.f)
		Strength = 0.f;

	//Making sure we at least output 5-10 damage
	if (MinDamage <= 0.f) 
	{
		MinDamage = BaseMinDamage;
	}
	
	if (MaxDamage<= 0.f) 
	{
		MaxDamage = BaseMaxDamage;
	}

	//Using a super generic formula to calculate AttackPower based on Strength attribute
	AttackPower = AttackPower + (Strength * 2);

	//Sending floor function as we aren't sending damage with decimals
	float WeaponOutputDamage = FMath::Floor(FMath::FRandRange(MinDamage, MaxDamage));

	//Once we have WeaponOutputDamage we increment based on attackpower
	WeaponOutputDamage = WeaponOutputDamage + (AttackPower / 4);

	//Formulas based off WoW Classic Damage Absorbtion formulas
	//Thought on implementing a slightly complex damage calculation for demo purposes
	//with this you can see that you can use damage calculations as complex as a RPG with lots of attributes
	//but also can be used as simple as just source.ATTACK - target.DEFENSE

	float DamageAbsorbtionPercentage = (Defense / (85 * attackerLevel + Defense + 400));

	float Damage = FMath::Floor(WeaponOutputDamage * (1 - DamageAbsorbtionPercentage));

	// For debug purposes only: may want to check the "output" damage if make any changes to the damage calculation logic
	// and compare with actual health changes in-engine to ensure numbers are correct
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("CALCULATED DAMAGE: %f"), Damage));

	if (Damage < 0.f) {
		Damage = 0.f;
	}

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(MeleeDamageStatics().HealthProperty, EGameplayModOp::Additive, -Damage));
}

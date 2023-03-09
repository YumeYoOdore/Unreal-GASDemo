// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.

#pragma once

#include "CoreMinimal.h" 
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GASAttributeSet.h"
#include "WeaponBase.h"
#include "DataTypes.h"
#include "GameplayEffect.h"
#include "CharacterBase.generated.h"

UCLASS(config = Game)
class GAS_DEMO_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

/*
* Class ACharacterBase
* The base class for characters: this is the default ThirdPerson template Base character default class
* but added custom functionality intended to be used in the demo, more specifically added
* GameplayAbilitySystem (GAS) functionalities.
* 
* Important notes on GAS
* 1) Characters that will implement GAS should implement the IAbilitySystemInterface
* 2) Characters MUST have an AbilitySystemComponent
* 2.1) When adding an AbilitySystemComponent make sure to override the GetAbiliitySystemComponent function
* 3) Add an attributeSet of your project's AttributeSet class
* Most of the previously defined points are usually boilerplate code, but is required to implement GAS on your project
* 
*/

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	/** AbilitySystemComponent */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponent* AbilitySystemComponent;

	/** Sample Attribute Set */
	UPROPERTY()
	const UGASAttributeSet* AttributeSet;

public:
	ACharacterBase();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	float TurnRateGamepad;


	//~ Begin IAbilitySystemInterface
	/** Returns our Ability System Component. Overriden from IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface

protected:
	virtual void BeginPlay();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	/** Utility function to create FGameplayModifierInfo for used with GameplayEffect
	* For use with OnEquipmentChanged, should NOT be called in blureprint
	*/
	FGameplayModifierInfo MakeEquipChangeModInfo(const float magniture, FGameplayAttribute AttributeToCapture);

	/** Utility function to check if the character is alive */
	UFUNCTION(BlueprintCallable)
	bool IsAlive();

	/** Utility used to ensure attributes are initialized only once */
	bool bAbilitiesInitialized;

	/** Character's level */
	int32 CharacterLevel;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Function to update stats after changing equipment. */
	UFUNCTION(BlueprintCallable)
	void OnEquipmentChanged(UWeaponBase* Equipment, EEquipmentChangeStatus EquipActionType);

	virtual void PossessedBy(AController* NewController) override;
	virtual void InitializeAttributes();
	virtual void GiveDefaultAbilities();

	// Utility function, based on Epic's ARPG demo function of the same name
	// Call this to "refresh" attributes when leveling up
	void RemoveStartupGameplayAbilities();

	/** Utility function used to reset the values of attributes defined in code */
	UFUNCTION(BlueprintCallable)
	virtual void ResetAttributes();

	// The following functions are used as getters from attributes defined in our AttributeSet
	// They are not mandatory but are really helpful when programming gameplay.
	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentHealth() const;

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentMana() const;

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxMana() const;

	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentStamina() const;

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable)
	virtual int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable)
	virtual bool SetCharacterLevel(int32 newLevel);

	/**
	* Called when character's HP changes
	*
	* @param DamageAmount Amount of damage that was done
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float DamageAmount);

	/**
	* Called when character's MP changes
	*
	* @param DeltaValue Value of Mana that changed
	* 
	*/

	UFUNCTION(BlueprintImplementableEvent)
	void OnManaChanged(float DeltaValue);

	/**
	* Called when character receives damage
	*
	* @param DamageAmount Amount of damage that was done
	* @param HitInfo Thr hit info that generated this damage
	* @param InstigatorCharacter The character that caused the damage
	* @param DamageCauser Actor that caused the damage
	*/
	UFUNCTION(BlueprintImplementableEvent)
	void OnDamaged(float DamageAmount, const FHitResult& HitInfo, ACharacterBase* InstigatorCharacter, AActor* DamageCauser);

	/* These functions are used as a "middle-point" between GameplayEffects that cause Attribute changes and any post - processing
	* events that take plase, usually on blueprints. 
	* Called in GASAttributeSet.PostGameplayEffectExecute() and in turns calls OnDamaged function Above which is BlueprintImplementable
	* then any display processing events can be handled on blueprint-side.
	* 
	* Important note: This function DOES NOT interfere with damage calculations and Attributes changes, that is handled
	* on SampleDamageExecution and AttributeSet respectively.
	* 
	* The same functionality can be found on Epic's ARPG demo, I just implemented my own version of it and tried to understand how it works
	* in order to document it approriately.
	*/
	void HandleDamage(float DamageAmount, const FHitResult& HitInfo, ACharacterBase* InstigatorCharacter, AActor* DamageCauser);

	void HandleHealthChanged(float DamageAmount);

	void HandleManaChanged(float DeltaValue);

	/** Container for GameplayEffects to take place at the start of game execution */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Abilities)
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	/** Container for the default abilities to be granted to the character at the start of game execution */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = Abilities)
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

};

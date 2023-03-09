// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "AttributeSet.h"

//////////////////////////////////////////////////////////////////////////
// ACharacterBase

ACharacterBase::ACharacterBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// NOTE: It is not Advisable to set camera here if we intend this class to be the Base class for every character
	// as some characters such as enemies should not have a camera
	// Leaving this here for simplicity sake as this is just expanding from the Third Person template

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));

	AttributeSet = CreateDefaultSubobject<UGASAttributeSet>(TEXT("Attributes"));

	CharacterLevel = 1;
	bAbilitiesInitialized = false;
}


void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Way 1 of initializing attributes, if starting from a DataTable use this for simplicity sake
	//if (AbilitySystemComponent)
	//	AttributeSet = AbilitySystemComponent->GetSet<UGASAttributeSet>();
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// Way 2 of initalizing attributes, use this if need more control when initializing attibutes
	InitializeAttributes();

	// Grant the character's AbilitySystemComponent it's default abilities
	GiveDefaultAbilities();
}

void ACharacterBase::InitializeAttributes()
{
	/* Function InitializeAttributes
	* Arguments: none
	* Output: none (Gives the character default attributes)
	*/

	if (AbilitySystemComponent && DefaultAttributeEffect)
	{
		/*
		* Modifying attributes with the GameplayAbilitySystem has to be done through a GameplayEffect,
		* in order to apply a GameplayEffect a context must be provided 
		* (FGameplayEffectContextHandle: this is initialized through AbylitySystemComponent->MakeEffectContext())
		* the attribute modifier are made through gameplay modifiers (FGameplayModifierInfo)
		* applied to the context handle (Effect->Modifiers.Add()).
		* 
		* This code attempts to retrieve the DefaultAttributeEffect values and create a SpecHandle (data structure)
		* if it is valid it proceeds to apply it to the AbilitySystemComponent (this portion affects AttributeSet on the
		* "backend")
		*/

		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, GetCharacterLevel(), EffectContext);

		if (SpecHandle.IsValid())
			FActiveGameplayEffectHandle GEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), AbilitySystemComponent);
	}
}

void ACharacterBase::GiveDefaultAbilities()
{
	/* Function GiveDefaultAbilities
	* Arguments: none
	* Output: none (Gives the character default abilities and sets bAbilitiesInitialized to true)
	*/

	// Make sure our AbilitySystemComponent exists and that we haven't initialized abilities
	if (AbilitySystemComponent && !bAbilitiesInitialized)
	{
		for (TSubclassOf<UGameplayAbility>& StartupAbility : DefaultAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility, GetCharacterLevel(), INDEX_NONE, this));
		}
	}

	bAbilitiesInitialized = true;
}

void ACharacterBase::RemoveStartupGameplayAbilities()
{
	/* Function RemoveStartupGameplayAbilities
	* Arguments: none
	* Output: none (Removes startup abilities as well as Effects)
	*/

	//Disclaimer: The following code is extracted from ARPG demo as we could consider this boilerplate
	//few modifications were made in order to accomodate it to this project.

	if (AbilitySystemComponent)
	{
		TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
		for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if ((Spec.SourceObject == this) && DefaultAbilities.Contains(Spec.Ability->GetClass()))
			{
				AbilitiesToRemove.Add(Spec.Handle);
			}
		}

		for (int i = 0; i < AbilitiesToRemove.Num(); i++) 
		{
			AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
		}

		FGameplayEffectQuery Query;
		Query.EffectSource = this;
		AbilitySystemComponent->RemoveActiveEffects(Query);

		bAbilitiesInitialized = false;
	}
}

void ACharacterBase::ResetAttributes()
{
	/* Function ResetAttributes
	* Arguments: none
	* Output: none (Resets the attributes specified below to their original value)
	*/

	if (AbilitySystemComponent && AttributeSet)
	{
		//Create an effect context and define the source object at this actor
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		// Learning-Tip: Try to review this portion of the code while reviewing the Blueprint version
		// of a GameplayEffect and try to find the parallelisms.
		UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("FullHeal")));

		FGameplayModifierInfo ModifierInfo;
		ModifierInfo.Attribute = AttributeSet->GetHealthAttribute();
		ModifierInfo.ModifierOp = EGameplayModOp::Override;
		ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(GetMaxHealth());

		Effect->Modifiers.Add(ModifierInfo);
		
		AbilitySystemComponent->ApplyGameplayEffectToSelf(Effect, 1, EffectContext);

	}
}

float ACharacterBase::GetCurrentHealth() const
{
	return AttributeSet->GetHealth();
}

float ACharacterBase::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}

float ACharacterBase::GetCurrentMana() const
{
	return AttributeSet->GetMana();
}

float ACharacterBase::GetMaxMana() const
{
	return AttributeSet->GetMaxMana();
}

float ACharacterBase::GetCurrentStamina() const
{
	return AttributeSet->GetStamina();
}

float ACharacterBase::GetMaxStamina() const
{
	return AttributeSet->GetMaxStamina();
}

int32 ACharacterBase::GetCharacterLevel() const
{
	return CharacterLevel;
}

bool ACharacterBase::SetCharacterLevel(int32 newLevel)
{
	/* Function SetCharacterLevel
	* Arguments: int32 newLevel - the character's new level
	* Output: true if sucessfully updated character's level, false on failure
	*/

	// Make sure we don't set character level to 0
	// also that the new level isn't the same as current level
	// we could also make sure we don't go BELOW current level
	// but there could always be events where we want to de-level
	// (like certain boss battles : make sure to give the player 
	// their original level back when ending the encounter or the
	// boss would be really unfair!)
	
	if (newLevel > 0 && newLevel != CharacterLevel)
	{
		//Based on EPIC's ARPG demo, we're removing abilities first and adding them again
		//to refresh attributes and stats
		RemoveStartupGameplayAbilities();
		CharacterLevel = newLevel;
		//Once we updated level initialize attributes and abilities again so they pick up the appropriate
		//level
		InitializeAttributes();
		GiveDefaultAbilities();

		return true;
	}

	return false;
}

/*
* The following functions are purely utility, HandleDamage is called when the owner character is dealt damage
* and then it calls blueprint implementable function OnDamaged so we can pickup the values in Blueprint
* the same holds for HandleHealthChanged and HandleManaChanged
* further read: GASAttributeSet.cpp
*/
void ACharacterBase::HandleDamage(float DamageAmount, const FHitResult& HitInfo, ACharacterBase* InstigatorCharacter, AActor* DamageCauser)
{
	OnDamaged(DamageAmount, HitInfo, InstigatorCharacter, DamageCauser);
}

void ACharacterBase::HandleHealthChanged(float DamageAmount)
{
	if (bAbilitiesInitialized)
	{
		OnHealthChanged(DamageAmount);
	}
}

void ACharacterBase::HandleManaChanged(float DeltaValue)
{
	if (bAbilitiesInitialized)
	{
		OnManaChanged(DeltaValue);
	}
}


UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


FGameplayModifierInfo ACharacterBase::MakeEquipChangeModInfo(const float magnitude, FGameplayAttribute AttributeToCapture)
{
	/* Function MakeEquipChangeModInfo
	* Arguments: const float magnitude - the target GameplayModifier magnitude value, FGameplayAttribute
	* AttributeToCapture - The attribute to be affected
	* Output: FGameplayModifierInfo to be applier on the FGameplayEffectContext to apply on the target's
	* AbilitySystemComponent
	*/

	FGameplayModifierInfo ModifierInfo;

	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(magnitude);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.Attribute = AttributeToCapture;

	return ModifierInfo;
}

void ACharacterBase::OnEquipmentChanged(UWeaponBase* Equipment, EEquipmentChangeStatus EquipActionType)
{
	/* Function OnEquipmentChanged
	* Arguments: UWeaponBase* Equipment - pointer to the new Equipment item to capture attributes from,
	* EEquipmentChangeStatus EquipActionType - Enum class value of the action to perform (Equip or UnEquip)
	* Output: none (when equipping a new weapon apply it's stats to the Owner's character AbilitySystem
	* for calculations handling on combat)
	*/

	//Variables to capture equipment stats and apply stats changes to character
	//For Stats like Health, Stamina, Mana (that have a separate current value for 
	//gameplay calculations) we only affect max value.
	//This could also be handled as a struct or even a map, but doing it this way
	//for simplicity sake.

	float Health = 0.f;
	float Stamina = 0.f;
	float Mana = 0.f;
	float AttackPower = 0.f;
	float Defense = 0.f;
	float Strength = 0.f;
	float Dexterity = 0.f;
	float Vitality = 0.f;
	float Endurance = 0.f;
	float Intelligence = 0.f;
	float Mind = 0.f;
	float Agility = 0.f;
	float CriticalRate = 0.f;

	float MinWeaponDamage = 0.f;
	float MaxWeaponDamage = 0.f;

	if (!Equipment) return;

	int32 AttributModCoefficient = 1;
	if (EquipActionType == EEquipmentChangeStatus::Unequip)
		AttributModCoefficient = -1;

	//float HealthIncrease = Equipment->GetMaxHealthIncrease() * AttributModCoefficient;
	//float DamageIncrease = Equipment->GetDamage() * AttributModCoefficient;

	//Capture attributes from equipment

	Health = Equipment->Health;
	Stamina = Equipment->Stamina;
	Mana = Equipment->Mana;

	//Raw attack power : This can also be calculated later on through strength
	AttackPower = Equipment->AttackPower;
	Defense = Equipment->Defense;
	Strength = Equipment->Strength;
	Dexterity = Equipment->Dexterity;
	Vitality = Equipment->Vitality;
	Endurance = Equipment->Endurance;
	Intelligence = Equipment->Intelligence;
	Mind = Equipment->Mind;
	Agility = Equipment->Agility;
	CriticalRate = Equipment->CriticalRate;

	//Weapon damage section, used for damage calculations : Beware that to have this functionality
	//we must first ensure our equipment item is a weapon (HINT: Probably add a equipmentType variable)
	MinWeaponDamage = Equipment->Damage.MinDamage;
	MaxWeaponDamage = Equipment->Damage.MaxDamage;


	if (AbilitySystemComponent && AttributeSet)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("EquipmentEffect")));

		//Stats attributes modifiers
		Effect->Modifiers.Add(MakeEquipChangeModInfo(Strength * AttributModCoefficient, AttributeSet->GetStrengthAttribute()));
		Effect->Modifiers.Add(MakeEquipChangeModInfo(Dexterity * AttributModCoefficient, AttributeSet->GetDexterityAttribute()));
		Effect->Modifiers.Add(MakeEquipChangeModInfo(Vitality * AttributModCoefficient, AttributeSet->GetVitalityAttribute()));
		Effect->Modifiers.Add(MakeEquipChangeModInfo(Endurance * AttributModCoefficient, AttributeSet->GetEnduranceAttribute()));
		Effect->Modifiers.Add(MakeEquipChangeModInfo(Intelligence * AttributModCoefficient, AttributeSet->GetIntelligenceAttribute()));
		Effect->Modifiers.Add(MakeEquipChangeModInfo(Mind * AttributModCoefficient, AttributeSet->GetMindAttribute()));
		Effect->Modifiers.Add(MakeEquipChangeModInfo(Agility * AttributModCoefficient, AttributeSet->GetAgilityAttribute()));

		Effect->Modifiers.Add(MakeEquipChangeModInfo(MinWeaponDamage * AttributModCoefficient, AttributeSet->GetMinWeaponDamageAttribute()));
		Effect->Modifiers.Add(MakeEquipChangeModInfo(MaxWeaponDamage * AttributModCoefficient, AttributeSet->GetMaxWeaponDamageAttribute()));

		//Raw attributes modifiers: these can be increased through equipment directly but also calculated
		//based on current stats
		Effect->Modifiers.Add(MakeEquipChangeModInfo(AttackPower * AttributModCoefficient, AttributeSet->GetAttackPowerAttribute()));
		Effect->Modifiers.Add(MakeEquipChangeModInfo(Defense * AttributModCoefficient, AttributeSet->GetDefenseAttribute()));
		Effect->Modifiers.Add(MakeEquipChangeModInfo(CriticalRate * AttributModCoefficient, AttributeSet->GetCriticalRateAttribute()));

		//Base attributes modifiers: these can be increased through equipment directly but also calculated
		//based on current stats
		Effect->Modifiers.Add(MakeEquipChangeModInfo(Health * AttributModCoefficient, AttributeSet->GetMaxHealthAttribute()));
		Effect->Modifiers.Add(MakeEquipChangeModInfo(Stamina * AttributModCoefficient, AttributeSet->GetMaxStaminaAttribute()));
		Effect->Modifiers.Add(MakeEquipChangeModInfo(Mana * AttributModCoefficient, AttributeSet->GetMaxManaAttribute()));

		//When done with creating all modifiers for all attributes, apply effect to self
		AbilitySystemComponent->ApplyGameplayEffectToSelf(Effect, 1, EffectContext);
	}
}

bool ACharacterBase::IsAlive()
{
	/* Function IsAlive
	* Arguments: none
	* Output: True if character's health is > 0; false otherwise
	*/

	//Small utility function to check if our character is currently dead or alive (DOA lol)
	float CurrentHealth = GetCurrentHealth();
	if (CurrentHealth <= 0.f)
		return false;

	return true;
}

// NOTE: The following code portion is the defaul't ThirdPerson template input code
// this could be handled through Blueprints or a separate class as some Character
// wouldn't require an input (Enemies, NPCs). Leaving this here for simplicity sake.

//////////////////////////////////////////////////////////////////////////
// Input

void ACharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ACharacterBase::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ACharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ACharacterBase::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACharacterBase::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ACharacterBase::TouchStopped);
}

void ACharacterBase::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ACharacterBase::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ACharacterBase::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


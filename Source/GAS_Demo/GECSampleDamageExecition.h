// Copyright & Fair Use Notice: This project is for educational and informational purposes only.  (C) 2023 - Gabriel Loaeza.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GECSampleDamageExecition.generated.h"

/**
 * 
 */
UCLASS()
class GAS_DEMO_API UGECSampleDamageExecition : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
/*
* Class SampleDamageExecution (I deeply apologize for the typo above, just realized when documenting the project's code)
* Use this class if a custom damage calculation is to be used when affecting an attribute rather than sending directly
* a hardcoded magnitude value.
*/
public:

	UGECSampleDamageExecition();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};

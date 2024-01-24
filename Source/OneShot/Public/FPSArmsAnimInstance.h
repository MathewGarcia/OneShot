// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "FPSArmsAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ONESHOT_API UFPSArmsAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "IsFiring")
	bool bIsFiring;

};

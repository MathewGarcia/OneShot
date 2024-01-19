// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
class AEnemy;
UCLASS()
class ONESHOT_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()


public:
	virtual void BeginPlay() override;

protected:
	virtual void Tick(float DeltaTime) override;

private:
	AEnemy* Enemy;
	
};

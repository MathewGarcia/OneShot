// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "CrawlerEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ONESHOT_API ACrawlerEnemy : public AEnemy
{
	GENERATED_BODY()

protected:
	virtual void AttackPlayer() override;

	virtual bool CanAttack() override;

	virtual void Tick(float DeltaTime) override;

};

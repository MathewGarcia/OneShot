// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Projectile.h"
#include "FlyingEnemy.generated.h"

/**
 * 
 */
UCLASS()
class ONESHOT_API AFlyingEnemy : public AEnemy
{
	GENERATED_BODY()
public:
	AFlyingEnemy();

	UPROPERTY(EditAnywhere,Category = "GroundDistance")
	float MaxGroundDistance;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<AProjectile>ProjectileToSpawn;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float FiringDistance;

protected:
	virtual void AttackPlayer() override;

	virtual bool CanAttack() override;

	virtual void Tick(float DeltaTime) override;

	float CheckDistanceToGround();

};

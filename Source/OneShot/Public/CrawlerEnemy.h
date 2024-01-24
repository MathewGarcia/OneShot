// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "CrawlerEnemy.generated.h"

/**
 * 
 */
class UCapsuleComponent;
class UNiagaraSystem;
class ADOTVolume;
UCLASS()
class ONESHOT_API ACrawlerEnemy : public AEnemy
{
	GENERATED_BODY()

	ACrawlerEnemy();
public:

	UPROPERTY(EditAnywhere,Category = "Attack Sphere")
	UCapsuleComponent* AttackCapsule;

	UPROPERTY(EditAnywhere,Category = "Stinky")
	UNiagaraSystem*Stinky;

	UPROPERTY(EditAnywhere, Category = "Stinky")
	TSubclassOf<ADOTVolume>StinkyDV;

	UFUNCTION()
	void OnAttackHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void AttackPlayer() override;

	virtual bool CanAttack() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Initialize() override;

	virtual void BeginPlay() override;

};

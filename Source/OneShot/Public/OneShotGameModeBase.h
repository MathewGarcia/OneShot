// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Weapon.h"
#include "ObjectPool.h"
#include "Enemy.h"
#include "OneShotGameModeBase.generated.h"

/**
 * 
 */
class AEnemyAIController;
class APlayerCharacter;
class UPlayerUIWidget;
UCLASS()
class ONESHOT_API AOneShotGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "Weapons")
	TArray<TSubclassOf<AWeapon>>WeaponsToSpawn;

	UPROPERTY(EditAnywhere, Category = "Weapons");
	TArray<TSubclassOf<AEnemy>> EnemiesToSpawn;

	UPROPERTY(EditAnywhere, Category = "Spawn Area")
	class ASpawnArea* SpawnArea;

	UPROPERTY(EditAnywhere, Category = "ObjectPooling")
	int AmountToSpawn;

	UPROPERTY(EditAnywhere, Category = "ObjectPooling")
	int AmountEnemiesToSpawn;

	UPROPERTY(EditAnywhere, Category = "GameModeRules")
	int MaxWeaponsAllowed;

	UPROPERTY(EditAnywhere, Category = "GameModeRules")
	int MaxEnemiesAllowed;

	UPROPERTY(EditAnywhere, Category = "GameModeRules")
	float EnemySpawnTime;

	UPROPERTY(EditAnywhere, Category = "Particles")
	UParticleSystem* EnemySpawnEmitter;

	UPROPERTY(EditAnywhere, Category = "AI")
	AEnemyAIController* AIController;

	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<AEnemyAIController> EnemyAIController;

	UPROPERTY(EditAnywhere,Category = "Sounds")
	USoundBase*BackgroundMusic;

	UPROPERTY(EditAnywhere, Category = "GameInfo")
	int PlayerScore;

	void SetWeapon();

	void SetEnemy();

	void SetCurrentWeaponAmount(int CurrentInGame);

	int GetCurrentWeaponAmount();

	void SetCurrentEnemiesAmount(int CurrentInGame);

	int GetCurrentEnemyAmount();

	void ActivateAICharacter(AEnemy* AIEnemy);

	void EndGame();

	float CurrentTime;

	TObjectPool<AWeapon>*WeaponPool;
	TObjectPool<AEnemy>* EnemyPool;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

private:
	int CurrentWeaponsInGame = 0;

	int CurrentEnemiesInGame = 0;

	FTimerHandle SpawnTimerHandle;

	APlayerCharacter* player;

	float TargetTime = 120;

	float MinSpawnTime = 5.f;


};

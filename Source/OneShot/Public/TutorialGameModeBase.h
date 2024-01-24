// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ObjectPool.h"
#include "TutorialGameModeBase.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ETutorialState : uint8 {
	MOVEMENT UMETA(DisplayName = "MOVEMENT"),
	LOOK UMETA(DisplayName = "LOOK"),
	SPRINT UMETA(DisplayName = "SPRINT"),
	SLIDE UMETA(DisplayName = "SLIDE"),
	CROUCH UMETA(DisplayName = "CROUCH"),
	JUMP UMETA(DisplayName = "JUMP"),
	FIRE UMETA(DisplayName = "FIRE"),
	PICKUP UMETA(DisplayName = "PICKUP"),
	FINAL UMETA(DisplayName = "FINAL"),
	NONE UMETA(DisplayName = "NONE"),

};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTutorialEventDelegate, ETutorialState, newTutorialState);

class APlayerCharacterController;
class AEnemy;
class ASpawnArea;
class AWeapon;
UCLASS()
class ONESHOT_API ATutorialGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void SetTutorialState(ETutorialState newTutorialState);

	FTutorialEventDelegate OnTutorialEvent;

	ETutorialState GetTutorialState() {
		return TutorialState;
	}

	UPROPERTY(EditAnywhere,Category = "Enemies")
	TArray<TSubclassOf<AEnemy>>EnemiesToSpawn;

	UPROPERTY(EditAnywhere, Category = "Weapons")
	TArray<TSubclassOf<AWeapon>>WeaponsToSpawn;


	UPROPERTY(EditAnywhere, Category = "Amount To Spawn")
	int AmountToSpawn;

	UFUNCTION(BlueprintCallable, Category = "EndGame")
	void EndGame();

	ETutorialState GetCurrentTutorialState();

	TObjectPool<AWeapon>* WeaponPool;
	TObjectPool<AEnemy>*EnemyPool;

	ASpawnArea* SpawnArea;

	TArray<ASpawnArea*>WeaponSpawnAreas;

	void SpawnEnemy();

	void SpawnWeapon(ASpawnArea*AtSpawn);

private:

	APlayerCharacterController* APCC;

	APlayerCharacter* player;

	ETutorialState TutorialState;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	void ActivateAICharacter(AEnemy* AIEnemy);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HitByProjectileInterface.h"
#include "Enemy.generated.h"

UENUM(BlueprintType)
enum class EAIStates : uint8 {
	IDLE UMETA(DisplayName = "IDLE"),
	SEARCHING UMETA(DisplayName = "PATROLLING"),
	ATTACKING UMETA(DisplayName = "ATTACKING"),
	COVER UMETA(DisplayName = "Cover")
};

class AOneShotGameModeBase;
class UNiagaraComponent;
UCLASS(Abstract)
class ONESHOT_API AEnemy : public ACharacter, public IHitByProjectileInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemy();

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Health")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Points")
	int pointsGiven;


	void SetHealth(float NewHealth);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MaxDistanceAllowed;

	UPROPERTY(EditAnywhere, BlueprintReadWRite, Category = "AI")
	float SpawnedHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWRite, Category = "AI")
	UNiagaraComponent*ParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemySounds")
	TArray<USoundBase*>AttackSounds;

	void MoveTowardsPlayer(float DeltaTime);

	//this is the same as making a function pure_virtual in C++ virtual void AttackPlayer() = 0;
	virtual void AttackPlayer() PURE_VIRTUAL(AEnemy::AttackPlayer, );

	virtual void Initialize() PURE_VIRTUAL(AEnemy::Initialize, );

	virtual bool CanAttack() PURE_VIRTUAL(AEnemy::CanAttack, return false;);

	FTimerHandle AttackSpeedTimerHandle;

	UFUNCTION(BlueprintCallable,Category = "AIState")
	EAIStates GetAIState();

	void SetAIState(EAIStates newState);

	void SetActivate(bool isActive);

	bool IsActive() const {
		return bisActive;
	}

	void SpawnEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	EAIStates EnemyState;
	class APlayerCharacter* player;

	bool bisActive;

	AOneShotGameModeBase* GM;

	float LastTimeTalked = 0.f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	virtual void HitByProjectile(float DamageAmount,AActor* Shooter, FDamageEvent& DamageEvent) override;

	bool MaxDistanceToPlayer();

private:
	FTimerHandle FRespawnTimerHandle;

};

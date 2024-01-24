// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingEnemy.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"


AFlyingEnemy::AFlyingEnemy()
{
	//set the enemy to flying mode.
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void AFlyingEnemy::AttackPlayer()
{
	int RandomNum = FMath::RandRange(0, 2);
	if (AttackSounds[RandomNum]) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSounds[RandomNum], GetActorLocation());
	}
	//spawn a projectile that will hit the player eventually, maybe follow the player for x seconds and then go nullptr
	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector();
	FVector Direction = (player->GetActorLocation() - SpawnLocation).GetSafeNormal();
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	AProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(ProjectileToSpawn, SpawnLocation, Direction.Rotation(), SpawnParams);
	SpawnedProjectile->FireInDirection(Direction*FiringDistance);
	MoveIgnoreActorAdd(SpawnedProjectile);
	ParticleSystem->ActivateSystem();
	GetWorld()->GetTimerManager().SetTimer(AttackSpeedTimerHandle, AttackSpeed, false);
}

bool AFlyingEnemy::CanAttack()
{
	if (player) {
		return FVector::Distance(player->GetActorLocation(), GetActorLocation()) <= AttackRange && !GetWorld()->GetTimerManager().IsTimerActive(AttackSpeedTimerHandle);
	}
	return false;
}

void AFlyingEnemy::Tick(float DeltaTime)
{
	if (CanAttack()) {
		EnemyState = EAIStates::ATTACKING;
	}
	else if (!CanAttack()  && EnemyState != EAIStates::SEARCHING) {
		EnemyState = EAIStates::SEARCHING;
	}

	float CurrentHeight = CheckDistanceToGround();

	//we also want to rotate the enemy towards our player NOT IMPLEMENTED YET

	if (CurrentHeight < MaxGroundDistance) {
		FVector Direction = FVector(0.f, 0.f, 1);
		AddMovementInput(Direction, 1.0f);
	}
	else if (CurrentHeight >= MaxGroundDistance) {
		FVector Direction = FVector(0.f, 0.f, -1);
		AddMovementInput(Direction, 1.0f);
	}
}

void AFlyingEnemy::Initialize()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	Health = SpawnedHealth;
}



float AFlyingEnemy::CheckDistanceToGround()
{
	//setting up line trace for our enemy
	FVector Start = GetActorLocation();
	//line trace from our location all the way down tot he max ground distance
	FVector End = Start - FVector(0.f, 0.f, MaxGroundDistance);

	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams)) {
		//get the difference from our location to the hit location and return the magnitude
		return (Start - Hit.Location).Size();
	}
	//if we dont hit we are clearly at max distance our higher
	return MaxGroundDistance;
}


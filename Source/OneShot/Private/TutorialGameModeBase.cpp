// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialGameModeBase.h"
#include "EnemyAIController.h"
#include "PlayerCharacterController.h"
#include "Enemy.h"
#include "FlyingEnemy.h"
#include "SpawnArea.h"
#include "Weapon.h"

void ATutorialGameModeBase::SetTutorialState(ETutorialState newTutorialState)
{

		TutorialState = newTutorialState;
		if (APCC) {
			switch (TutorialState) {
			case ETutorialState::MOVEMENT:
				APCC->SetTutorialText(FText::FromString("Use W,A,S,D to move"));
				break;
			case ETutorialState::LOOK:
				APCC->SetTutorialText(FText::FromString("Use your mouse to look around"));
				break;
			case ETutorialState::SPRINT:
				APCC->SetTutorialText(FText::FromString("Press and Hold Left Shift to Sprint"));
				break;
			case ETutorialState::SLIDE:
				APCC->SetTutorialText(FText::FromString("While sprinting , press C to slide"));
				break;
			case ETutorialState::CROUCH:
				APCC->SetTutorialText(FText::FromString("Press C to crouch"));
				break;
			case ETutorialState::JUMP:
				APCC->SetTutorialText(FText::FromString("Press SPACE BAR to Jump, you can double jump"));
				break;
			case ETutorialState::FIRE:
				APCC->SetTutorialText(FText::FromString("Press Left click to fire"));
				break;
			case ETutorialState::PICKUP:
				APCC->SetTutorialText(FText::FromString("To pick up a weapon, walk over it. Pick up weapons only have 1 bullet. So be careful."));
				break;
			case ETutorialState::FINAL://MAY ADD FIRING AT THE PROJECTILE AND DESTROYING IT GIVES YOU ANOTHER SHOT
				APCC->SetTutorialText(FText::FromString("If you shoot a projectile mid air, you will get another shot back (cannot be your default weapon and cannot be a projectile weapon)"));
				break;
			case ETutorialState::NONE:
				break;
			}
		}
}

ETutorialState ATutorialGameModeBase::GetCurrentTutorialState()
{
	return TutorialState;
}

void ATutorialGameModeBase::SpawnEnemy()
{
		AEnemy* Enemy = EnemyPool->GetPooledObject(AFlyingEnemy::StaticClass());
		if (SpawnArea) {
			if (Enemy) {
				Enemy->SetActorLocation(SpawnArea->GetActorLocation());
				ActivateAICharacter(Enemy);
				Enemy->Initialize();
				Enemy->MaxDistanceAllowed = 1000;
			}
		}
}

void ATutorialGameModeBase::SpawnWeapon(ASpawnArea*AtSpawn)
{
	UE_LOG(LogTemp, Warning, TEXT("Spawning Weapon"));
	if (AtSpawn) {
		AWeapon* Weapon = WeaponPool->GetPooledObject(AtSpawn->WeaponToCheck);
		if (Weapon) {
			AtSpawn->CurrentSpawnedWeapon = Weapon;
			Weapon->SetActorLocation(AtSpawn->GetActorLocation());
			Weapon->Initialize();
		}
	}
}


void ATutorialGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	APCC = Cast<APlayerCharacterController>(GetWorld()->GetFirstPlayerController());
	OnTutorialEvent.AddDynamic(this, &ATutorialGameModeBase::SetTutorialState);
	SetTutorialState(ETutorialState::MOVEMENT);
	EnemyPool = new TObjectPool<AEnemy>;
	EnemyPool->InitPool(EnemiesToSpawn, AmountToSpawn, GetWorld());

	WeaponPool = new TObjectPool<AWeapon>;
	WeaponPool->InitPool(WeaponsToSpawn, AmountToSpawn, GetWorld());
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
		for (int i = 0; i < WeaponSpawnAreas.Num(); i++) {
			SpawnWeapon(WeaponSpawnAreas[i]);
		}
		
		}, 1.0f, false);

}

void ATutorialGameModeBase::Tick(float DeltaTime)
{

}

void ATutorialGameModeBase::ActivateAICharacter(AEnemy* AIEnemy)
{
	if (AIEnemy) {
		AEnemyAIController* Controller = Cast<AEnemyAIController>(AIEnemy->GetController());
		if (!Controller) {
			Controller = GetWorld()->SpawnActor<AEnemyAIController>(AEnemyAIController::StaticClass());
			if (Controller) {
				Controller->Possess(AIEnemy);
				Controller->SetEnemy(AIEnemy);
			}
		}

	}
}
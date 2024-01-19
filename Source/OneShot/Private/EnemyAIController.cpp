// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Enemy.h"

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	Enemy = Cast<AEnemy>(GetPawn());
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Enemy) {
		switch (Enemy->GetAIState()) {
		case EAIStates::IDLE:
		case EAIStates::SEARCHING:
			Enemy->MoveTowardsPlayer(DeltaTime);
			break;
		case EAIStates::ATTACKING:
			Enemy->AttackPlayer();
			break;
		}
	}
}

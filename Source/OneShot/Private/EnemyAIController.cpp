// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "Enemy.h"
#include "NiagaraComponent.h"
#include "NavigationSystem.h"
#include "PlayerCharacter.h"

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	Enemy = Cast<AEnemy>(GetPawn());
	player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void AEnemyAIController::SetEnemy(AEnemy* newEnemy)
{
	Enemy = newEnemy;
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
		default: 
			break;
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Enemy failed in Enemy AI tick"));

	}
}

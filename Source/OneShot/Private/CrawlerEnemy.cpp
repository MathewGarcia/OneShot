// Fill out your copyright notice in the Description page of Project Settings.


#include "CrawlerEnemy.h"
#include "PlayerCharacter.h"
#include "Enemy.h"
#include "PlayerCharacter.h"

void ACrawlerEnemy::AttackPlayer()
{
	//attack the player
	UE_LOG(LogTemp, Warning, TEXT("Crawler Enemy is attacking !"));
	GetWorld()->GetTimerManager().SetTimer(AttackSpeedTimerHandle, AttackSpeed, false);

}

bool ACrawlerEnemy::CanAttack()
{
	if (player) {
		return FVector::Distance(player->GetActorLocation(), GetActorLocation()) <= AttackRange && !GetWorld()->GetTimerManager().IsTimerActive(AttackSpeedTimerHandle);
	}
	return false;
}

void ACrawlerEnemy::Tick(float DeltaTime)
{
	if (CanAttack()) {
		EnemyState = EAIStates::ATTACKING;
	}
	else if (!CanAttack() && EnemyState != EAIStates::SEARCHING) {
		EnemyState = EAIStates::SEARCHING;
	}
}

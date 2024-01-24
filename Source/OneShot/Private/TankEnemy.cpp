// Fill out your copyright notice in the Description page of Project Settings.


#include "TankEnemy.h"
#include "PlayerCharacter.h"
#include "Engine/DamageEvents.h"
#include "Engine/EngineTypes.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

void ATankEnemy::AttackPlayer()
{

	int RandomNum = FMath::RandRange(0, 2);
	if (AttackSounds[RandomNum]) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSounds[RandomNum], GetActorLocation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetActorLocation());

	}
	//shoot fire
	FVector Start = GetActorLocation();
	FVector End = Start + GetActorForwardVector() * 500.f;
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredComponent(GetMesh());
	CollisionParams.AddIgnoredComponent(Cast<UPrimitiveComponent>(GetCapsuleComponent()));
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams)) {

		if (Hit.bBlockingHit) {
			if (APlayerCharacter* HitPlayer = Cast<APlayerCharacter>(Hit.GetActor())) {
				FRadialDamageEvent RadialDamage;
				RadialDamage.Params.BaseDamage = Damage;
				RadialDamage.Params.InnerRadius = 2.f;
				RadialDamage.Params.OuterRadius = 4.f;
				RadialDamage.Params.DamageFalloff = 1.0f;
				RadialDamage.Params.MinimumDamage = Damage / 2;
				HitPlayer->TakeDamage(Damage,RadialDamage,GetController(),this);
				DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.0f, 0, 1.0f);

			}
		}
	}

		ParticleSystem->ActivateSystem();
	GetWorld()->GetTimerManager().SetTimer(AttackSpeedTimerHandle, AttackSpeed, false);

}

bool ATankEnemy::CanAttack()
{
	if (player) {
		return FVector::Distance(player->GetActorLocation(), GetActorLocation()) <= AttackRange && !GetWorld()->GetTimerManager().IsTimerActive(AttackSpeedTimerHandle);
	}
	return false;
}

void ATankEnemy::Tick(float DeltaTime)
{
	if (CanAttack() && EnemyState != EAIStates::ATTACKING) {
		EnemyState = EAIStates::ATTACKING;
	}
	else if (!CanAttack() && EnemyState != EAIStates::SEARCHING) {
		EnemyState = EAIStates::SEARCHING;
	}
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - LastTimeTalked >= 10.f) {
		int RandomNum = FMath::RandRange(0, 2);
		if (AttackSounds[RandomNum]) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSounds[RandomNum], GetActorLocation());
		}
		LastTimeTalked = GetWorld()->GetTimeSeconds();

	}
}

void ATankEnemy::Initialize()
{
	Health = SpawnedHealth;
}

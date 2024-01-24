// Fill out your copyright notice in the Description page of Project Settings.


#include "CrawlerEnemy.h"
#include "PlayerCharacter.h"
#include "Enemy.h"
#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/EngineTypes.h"
#include "Kismet/GameplayStatics.h"

ACrawlerEnemy::ACrawlerEnemy()
{
	AttackCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Attack Capsule"));
	AttackCapsule->SetupAttachment(GetMesh(), FName("Attack Sphere"));
	AttackCapsule->SetGenerateOverlapEvents(true);
	AttackCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACrawlerEnemy::OnAttackHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	if (OtherActor) {
		if (APlayerCharacter* HitPlayer = Cast<APlayerCharacter>(OtherActor)) {
			UE_LOG(LogTemp, Warning, TEXT("HIT WITH %f DAMAGE"), Damage);
				HitPlayer->TakeDamage(Damage, FDamageEvent(), GetController(), this);
		}
	}
}

void ACrawlerEnemy::AttackPlayer()
{
	//attack the player
	UE_LOG(LogTemp, Warning, TEXT("Crawler Enemy is attacking !"));
	int RandomNum = FMath::RandRange(0, 2);
	if(AttackSounds[RandomNum]){
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), AttackSounds[RandomNum], GetActorLocation());
	}
	AttackCapsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	GetWorld()->GetTimerManager().SetTimer(AttackSpeedTimerHandle, AttackSpeed, false);
	//shut off collision after attacking.
	FTimerHandle ShutOffCollision;
	GetWorld()->GetTimerManager().SetTimer(ShutOffCollision, [this]() {
		AttackCapsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

		}, 0.5, false);

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

void ACrawlerEnemy::Initialize()
{
	Health = SpawnedHealth;
}

void ACrawlerEnemy::BeginPlay()
{
	Super::BeginPlay();
	AttackCapsule->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("AttackCapsule"));
	AttackCapsule->SetWorldRotation(FRotator(90, 0, 0));
	AttackCapsule->SetRelativeLocation(FVector(-AttackCapsule->GetScaledCapsuleHalfHeight(), 0, 0));
	AttackCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackCapsule->OnComponentBeginOverlap.AddDynamic(this, &ACrawlerEnemy::OnAttackHit);
}

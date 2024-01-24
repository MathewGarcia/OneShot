// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Weapon.h"
#include "PlayerCharacter.h"
#include "OneShotGameModeBase.h"
#include "EnemyAIController.h"
#include "PlayerCharacterController.h"
#include "NiagaraComponent.h"
#include "CrawlerEnemy.h"
#include "NiagaraFunctionLibrary.h"
#include "DOTVolume.h"
#include "FlyingEnemy.h"
// Sets default values
AEnemy::AEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EnemyState = EAIStates::IDLE;
	ParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle System"));
	ParticleSystem->SetupAttachment(GetMesh(), FName("ParticleSocket"));
	ParticleSystem->SetAutoActivate(false);
}

void AEnemy::SetHealth(float NewHealth)
{
	Health = NewHealth;
}

void AEnemy::MoveTowardsPlayer(float DeltaTime)
{
	if (player) {
		//get the players location
		FVector PlayerLocation = player->GetActorLocation();
		float CurrentTime = GetWorld()->GetTimeSeconds();
		FVector Direction;

		if (CurrentTime - LastChecked > 1.0f) {
			Direction = WallCheck();
		}
		else {
			//get the direction by subtracting the 2 points in space and normalizing
			Direction = (PlayerLocation - GetActorLocation()).GetSafeNormal();
		}
		//now that we have our direction make a rotation out of it
		FRotator RotationDirection = FRotationMatrix::MakeFromX(Direction).Rotator();
		//interpolation to rotate our enemy over time
		FRotator EnemyRotation = FMath::RInterpTo(GetActorRotation(), RotationDirection, DeltaTime, 7.f );
		if (!MaxDistanceToPlayer()) {
			//move our enemy
			AddMovementInput(Direction, 1.0f);
		}
		if (this->IsA(AFlyingEnemy::StaticClass())) {
			//set the rotation
			SetActorRotation(EnemyRotation);
		}
		else {
			SetActorRotation(FRotator(0.0f,EnemyRotation.Yaw,EnemyRotation.Roll));

		}
	}


}


EAIStates AEnemy::GetAIState()
{
	return EnemyState;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	 player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	 GM = Cast<AOneShotGameModeBase>(GetWorld()->GetAuthGameMode());

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	SetHealth(Health - DamageAmount);
	UE_LOG(LogTemp, Warning, TEXT("Hit Enemy with %f Damage"), DamageAmount);
	if (Health < 0.0f) {
		AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
		//since we are not using a pool of AI Controllers destroy the controller.
		if (AIController) {
			AIController->UnPossess();
			AIController->Destroy();
		}
		//die , send back to object pool
		if (GM) {
			//if the enemy that died is crawler enemy and we have been playing for over 2 min.
			if (this->IsA(ACrawlerEnemy::StaticClass()) && GetWorld()->GetTimeSeconds() > 120.f) {
				float RandomNum = FMath::RandRange(0, 5);
				//spawn a stinky kill volume.
				if (ACrawlerEnemy* CrawlerEnemy = Cast<ACrawlerEnemy>(this)) {
					if (CrawlerEnemy->Stinky && RandomNum <= 2.5f) {
						UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CrawlerEnemy->Stinky, GetActorLocation());
						if (CrawlerEnemy->StinkyDV) {
							ADOTVolume* SpawnedDOTVolume = GetWorld()->SpawnActor<ADOTVolume>(CrawlerEnemy->StinkyDV, GetActorTransform());
							FTimerHandle FStinkyTimerHandle;
							GetWorld()->GetTimerManager().SetTimer(FStinkyTimerHandle, [this, SpawnedDOTVolume]() {
								SpawnedDOTVolume->Destroy();
								}, 8.0f, false);
						}
					}
				}
			}

			if (player) {
				if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(player->GetController())) {
					GM->PlayerScore += pointsGiven;
					player->SetHealth(player->Health + 5);
					PCC->SetScore(GM->PlayerScore);
				}
			}
			GM->EnemyPool->ReturnPooledObject(this);
			//call to spawn another enemy.
			SpawnEnemy();
		}
		else if (ATutorialGameModeBase* TGM = Cast<ATutorialGameModeBase>(GetWorld()->GetAuthGameMode())) {
			TGM->EnemyPool->ReturnPooledObject(this);
		}
	}

	return DamageAmount;
}

void AEnemy::HitByProjectile(float DamageAmount,AActor* Shooter, FDamageEvent& DamageEvent)
{
	if (APlayerCharacter* playerShooter = Cast<APlayerCharacter>(Shooter)) {
		TakeDamage(DamageAmount, DamageEvent,playerShooter->GetController(), playerShooter);
	}
}

void AEnemy::SetActivate(bool isActive)
{
	bisActive = isActive;

}

void AEnemy::SetAIState(EAIStates newState)
{
	EnemyState = newState;
}

bool AEnemy::MaxDistanceToPlayer()
{
	return FVector::Dist(player->GetActorLocation(), GetActorLocation()) <= MaxDistanceAllowed;
}

FVector AEnemy::WallCheck()
{
	FVector PlayerLocation = player->GetActorLocation();
	FVector Start = GetActorLocation();
	FVector End = Start+GetActorForwardVector() * 80;
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params)) {
		if (Hit.bBlockingHit) {
			if (APlayerCharacter* PlayerInFront = Cast<APlayerCharacter>(Hit.GetActor())) {
				return (PlayerLocation - GetActorLocation()).GetSafeNormal();
			}
			return GetActorForwardVector().MirrorByVector(Hit.ImpactNormal);
		}
	}
	return (PlayerLocation - GetActorLocation()).GetSafeNormal();
}

void AEnemy::SpawnEnemy()
{
	if (GM) {
		GetWorld()->GetTimerManager().SetTimer(FRespawnTimerHandle, [this]() {
			GM->SetEnemy();
			},GM->EnemySpawnTime,false);
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "OneShotGameModeBase.h"
#include "ObjectPool.h"
#include "SpawnArea.h"
#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnemyAIController.h"
#include "Particles/ParticleSystemComponent.h"




void AOneShotGameModeBase::SetWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("Set Weapon"));
	int RandomNum = FMath::RandRange(0, WeaponsToSpawn.Num() - 1);

	AWeapon* Weapon = WeaponPool->GetPooledObject(WeaponsToSpawn[RandomNum]);
	if (Weapon) {
		//Set it in a random location in the map area.
		if (SpawnArea) {
			FVector Origin;
			FVector BoxExtent;
			SpawnArea->GetActorBounds(false,Origin,BoxExtent);
			FVector RandomPoint = Origin - BoxExtent + 2.0f * BoxExtent * FVector(FMath::FRand(), FMath::FRand(), FMath::FRand());
			FVector Start = FVector(RandomPoint.X, RandomPoint.Y, Origin.Z);
			FVector End = FVector(RandomPoint.X, RandomPoint.Y, -Origin.Z); //go to 0 basically the floor.
			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(SpawnArea);
			if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params)) {
				if (Hit.bBlockingHit) {
					if (AEnemy* EnemyHit = Cast<AEnemy>(Hit.GetActor()))
					{
						//dont want to spawn a weapon on an enemy.
						return;
					}
					if (APlayerCharacter* PlayerHit = Cast<APlayerCharacter>(Hit.GetActor())) {
						//also dont want it to spawn on our player.
						return;
					}
					Weapon->SetActorLocation(Hit.Location);
					Weapon->SetActorRotation(FRotator::ZeroRotator);
					Weapon->Initialize();
					CurrentWeaponsInGame++;
				}
				DrawDebugLine(GetWorld(), Start, End, FColor::Blue, true, 1.0f);
			}

		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Spawn area null"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Tried to spawn weapon , failed"));
	}
}

void AOneShotGameModeBase::SetEnemy()
{
	int RandomNum = FMath::RandRange(0, EnemiesToSpawn.Num() - 1);

	AEnemy*Enemy = EnemyPool->GetPooledObject(EnemiesToSpawn[RandomNum]);
	if (Enemy) {
		//Set it in a random location in the map area.
		if (SpawnArea) {
			FVector Origin;
			FVector BoxExtent;
			SpawnArea->GetActorBounds(false, Origin, BoxExtent);
			FVector RandomPoint = Origin - BoxExtent + 2.0f * BoxExtent * FVector(FMath::FRand(), FMath::FRand(), FMath::FRand());
			FVector Start = FVector(RandomPoint.X, RandomPoint.Y, Origin.Z);
			FVector End = FVector(RandomPoint.X, RandomPoint.Y, -Origin.Z); //go to 0 basically the floor.
			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.AddIgnoredActor(SpawnArea);
			if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params)) {
				if (Hit.bBlockingHit) {
					if (AEnemy* EnemyHit = Cast<AEnemy>(Hit.GetActor()))
					{
						//dont want to spawn a weapon on an enemy.
						return;
					}
					if (APlayerCharacter* PlayerHit = Cast<APlayerCharacter>(Hit.GetActor())) {
						//also dont want it to spawn on our player.
						return;
					}

					UParticleSystemComponent* SpawnedEmitter = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EnemySpawnEmitter, Hit.Location, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease, true);
					FTimerHandle TimerHandle;
					GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Hit, Enemy,SpawnedEmitter]() {	Enemy->SetAIState(EAIStates::SEARCHING);
					Enemy->SetActorLocation(Hit.Location + FVector(0.0f, 0.0f, 100.f));
					Enemy->SetActorRotation(FRotator::ZeroRotator);
					ActivateAICharacter(Enemy);
					Enemy->Initialize();
					SpawnedEmitter->DeactivateSystem();
					CurrentEnemiesInGame++; 
						}, 1.0f, false);

				}
				DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, true, 1.0f);
			}

		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Spawn area null"));
		}
	}

}

void AOneShotGameModeBase::SetCurrentWeaponAmount(int CurrentInGame)
{
	CurrentWeaponsInGame = CurrentInGame;
}

int AOneShotGameModeBase::GetCurrentWeaponAmount()
{
	return CurrentWeaponsInGame;
}

void AOneShotGameModeBase::ActivateAICharacter(AEnemy* AIEnemy)
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

void AOneShotGameModeBase::BeginPlay()
{
	WeaponPool = new TObjectPool<AWeapon>();
	WeaponPool->InitPool(WeaponsToSpawn, AmountToSpawn, GetWorld());

	EnemyPool = new TObjectPool<AEnemy>();
	EnemyPool->InitPool(EnemiesToSpawn, AmountEnemiesToSpawn, GetWorld());

	player = Cast<APlayerCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());


		//lambda function to call setweapon after 1 second.
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
			for (int i = 0; i < MaxWeaponsAllowed; i++) {
				SetWeapon();
			}
			for (int i = 0; i < MaxEnemiesAllowed; i++) {
				SetEnemy();
			}
			}, 1.0, false);
		if (BackgroundMusic) {
			UGameplayStatics::PlaySound2D(GetWorld(), BackgroundMusic);
		}
}

void AOneShotGameModeBase::Tick(float DeltaTime)
{
	CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime >= TargetTime) {
		TargetTime += 120;
		for (AEnemy* EnemyInPool : EnemyPool->GetAllPooledObjects()) {
			if (EnemyInPool && EnemyInPool->IsValidLowLevel()) {
				EnemyInPool->Damage += FMath::FloorToInt(EnemyInPool->Damage * 0.05);
				EnemyInPool->Health += FMath::FloorToInt(EnemyInPool->Health * 0.03);
			}
		}

		EnemySpawnTime = FMath::Max(MinSpawnTime,EnemySpawnTime - (EnemySpawnTime * 0.05));
		//increase the amount of enemies by 3% but never go over 50. This is for optimization
		MaxEnemiesAllowed = FMath::Min(MaxEnemiesAllowed * 1.03,50);
		//increase the amount of weapons by 5% but never go over 20.
		MaxWeaponsAllowed = FMath::Min(MaxWeaponsAllowed * 1.05,20);
	}
}

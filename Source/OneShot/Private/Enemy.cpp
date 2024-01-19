// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Weapon.h"
#include "PlayerCharacter.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EnemyState = EAIStates::IDLE;
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
		//get the direction by subtracting the 2 points in space and normalizing
		FVector Direction = (PlayerLocation - GetActorLocation()).GetSafeNormal();
		//now that we have our direction make a rotation out of it
		FRotator RotationDirection = FRotationMatrix::MakeFromX(Direction).Rotator();
		//interpolation to rotate our enemy over time
		FRotator EnemyRotation = FMath::RInterpTo(GetActorRotation(), RotationDirection, DeltaTime, 7.f );
		//move our enemy
		AddMovementInput(Direction, 1.0f);
		//set the rotation
		SetActorRotation(EnemyRotation);

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
		//die
		Destroy();
	}

	return DamageAmount;
}

void AEnemy::HitByProjectile(float DamageAmount,AActor* Shooter, FDamageEvent& DamageEvent)
{
	if (APlayerCharacter* playerShooter = Cast<APlayerCharacter>(Shooter)) {
		TakeDamage(DamageAmount, DamageEvent,playerShooter->GetController(), playerShooter);
	}
}

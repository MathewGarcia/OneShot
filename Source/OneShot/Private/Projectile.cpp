// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "Weapon.h"
#include "Enemy.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
	CollisionComponent->SetSphereRadius(5.0f);
	CollisionComponent->SetVisibility(true);
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->BodyInstance.SetCollisionProfileName("Projectile");

	RootComponent = CollisionComponent;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	
	PMC = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projecitle Movement Comp"));
	PMC->SetUpdatedComponent(GetCollisionComponent());
	PMC->InitialSpeed = 1500.f;
	PMC->MaxSpeed = 2500.f;
	PMC->bRotationFollowsVelocity = true;
	PMC->bShouldBounce = false;
	PMC->ProjectileGravityScale = 1.0f;
}

void AProjectile::FireInDirection(const FVector& Direction)
{
	InitialDirection = Direction.GetSafeNormal();
	PMC->Velocity = InitialDirection * PMC->InitialSpeed;

}

float AProjectile::CalcDamageFromDistance(AActor* Victim, float Distance)
{
	float DPSFalloff = 1.5f;
	float OuterRadius = 100.f;
	float DamageAmount;
	//player is the victim
	if (APlayerCharacter* PlayerVictim = Cast<APlayerCharacter>(Victim)) {
		//so the enemy is the shooter
		AEnemy* Enemy = Cast<AEnemy>(GetOwner());
		if (Enemy) {
			//set the max and minimum damage
			float MaxDamage = Enemy->Damage;
			float MinDamage = Enemy->Damage / 2;

			//clamp the damage to 0 or 1 will return a float between 0 and 1
			float DamageBasedOffHitpoint = FMath::Clamp((OuterRadius - Distance) / OuterRadius, 0.0f, 1.f);
			//then lerp the damage between the minimum and maximum multiplied by the damage fall off. Based off the alpha, this gives us either full damage or not.
			DamageAmount = FMath::Lerp(MinDamage, MaxDamage, DamageBasedOffHitpoint * DPSFalloff);
		}
	}
	else if (AEnemy* EnemyVictim = Cast<AEnemy>(Victim)){
		//so the shooter is the player
		APlayerCharacter* PlayerShooter = Cast<APlayerCharacter>(GetOwner());
		if (PlayerShooter) {
			float MaxDamage = PlayerShooter->GetCurrentWeapon()->DamageAmount;
			float MinDamage = PlayerShooter->GetCurrentWeapon()->DamageAmount/2;
			float DamageBasedOffHitpoint = FMath::Clamp((OuterRadius - Distance) / OuterRadius, 0.0f, 1.f);
			DamageAmount = FMath::Lerp(MinDamage, MaxDamage, DamageBasedOffHitpoint * DPSFalloff);
		}
	}
	return DamageAmount;
}



// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
	CollisionComponent->MoveIgnoreActors.Add(GetOwner());
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	float LerpRate = 5.0f;
	float CurrentSpeed = PMC->Velocity.Size();
	float NewSpeed = FMath::FInterpTo(CurrentSpeed, PMC->MaxSpeed, DeltaTime,LerpRate);
	PMC->Velocity = InitialDirection*NewSpeed;

}

void AProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	FRadialDamageEvent RadialDamageEvent;

	//this is a direct hit, if the other actor implements our interface, then give it full damage
	if (Other && Other->Implements<UHitByProjectileInterface>()) {
		IHitByProjectileInterface* HitActor = Cast<IHitByProjectileInterface>(Other);
		float Damage;
		if (HitActor) {
			if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner())) {
				 Damage = player->GetCurrentWeapon()->DamageAmount;
			}
			else if (AEnemy* Enemy = Cast<AEnemy>(GetOwner())) {
				 Damage = Enemy->Damage;
			}
			HitActor->HitByProjectile(Damage, GetOwner(), RadialDamageEvent);

			UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation(), FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);

			Destroy();
			return;
		}
	}

	//if the projectile missed, then do radius damage.
	TArray<AActor*> HitActors;
	//spawning a sphere getting actors in the spher
	UKismetSystemLibrary::SphereOverlapActors(this, Hit.ImpactPoint, 500.f, TArray<TEnumAsByte<EObjectTypeQuery>>(), AActor::StaticClass(), TArray<AActor*>(), HitActors);

	DrawDebugSphere(GetWorld(), GetActorLocation(), 500.f, 32,FColor::Red, false, 2.0f);

	for (AActor* Actor : HitActors) {
		//if the actor is not null and is implementing our interface 
		if (Actor && Actor->Implements<UHitByProjectileInterface>()) {
			//calculate the magnitude of our location - the hit point.
			float Distance = (Actor->GetActorLocation() - Hit.ImpactPoint).Size();
			//call our calc damage from distance
			float DamageAmount = CalcDamageFromDistance(Actor, Distance);
			//apply the damage.
			if (APlayerCharacter* playerHit = Cast<APlayerCharacter>(Actor)) {
				playerHit->HitByProjectile(DamageAmount,GetOwner(), RadialDamageEvent);
			}
			else if (AEnemy* EnemyHit = Cast<AEnemy>(Actor)) {
				EnemyHit->HitByProjectile(DamageAmount, GetOwner(), RadialDamageEvent);
			}
		}
	}
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, GetActorLocation(), FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);

	Destroy();
}


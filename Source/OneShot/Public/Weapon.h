// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
	HITSCAN UMETA(DisplayName = "Hitscan"),
	PROJECTILE UMETA(DisplayName = "ProjectileWeapon")
};

class AProjectile;
class UCollisionComponent;
class UNiagaraSystem;
class UNiagaraComponent;
UCLASS()
class ONESHOT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UPROPERTY(EditAnywhere,BlueprintReadWrite ,Category = "Weapon Mesh")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "Weapon Type")
	EWeaponType WeaponType;

	 EWeaponType GetWeaponType() const;

	 //dont really like this, i guess i should add an enum to determine what exact type it is...
	 UPROPERTY(EditAnywhere, Category = "Is Shotgun")
	 bool bIsShotgun;

	UPROPERTY(EditAnywhere, Category = "Weapon Particle")
	bool isHorizontalRPG;

	 UPROPERTY(EditAnywhere, Category = "Shells")
	 int Shells;

	 UPROPERTY(EditAnywhere, Category = "Weapon Info")
	 int MaxAmmo;

	 UPROPERTY(EditAnywhere, Category = "Weapon Info")
	 float DamageAmount;
	
	 UPROPERTY(EditAnywhere, Category = "Weapon Info")
	 float fireRate;

	 UPROPERTY(EditAnywhere, Category = "Weapon Info")
	 USoundBase*FiringSound;

	 UPROPERTY(EditAnywhere, Category = "Weapon Particle")
	 UNiagaraComponent*WeaponDrop;

	 int CurrentAmmo;

	 UPROPERTY(EditAnywhere, Category = "Weapon Particle")
	 UNiagaraSystem* WeaponParticle;

	 UPROPERTY(EditAnywhere, Category = "Weapon Info")
	 TSubclassOf<AProjectile>Projectile;

	 void Initialize();

	 void SetActivate(bool bActive);

	 bool IsActive() const {
		 return bisActive;
	 }
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool bisActive;
};

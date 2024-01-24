#include "Weapon.h"
#include "PlayerCharacter.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);
	WeaponMesh->SetGenerateOverlapEvents(true);
	RootComponent = WeaponMesh;

	WeaponDrop = CreateDefaultSubobject<UNiagaraComponent>(TEXT("WeaponDropParticle"));
	WeaponDrop->SetupAttachment(WeaponMesh);
}


EWeaponType AWeapon::GetWeaponType() const
{
	return WeaponType;
}

void AWeapon::Initialize()
{
	CurrentAmmo = MaxAmmo;
	SetActorScale3D(FVector(1.f, 1.f, 1.f));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	WeaponDrop->ActivateSystem();
}

void AWeapon::SetActivate(bool bActive)
{
	bisActive = bActive;
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	Initialize();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


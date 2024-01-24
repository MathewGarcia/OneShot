#include "DOTVolume.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "DOTVolume.h"
#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"
// Sets default values
ADOTVolume::ADOTVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetGenerateOverlapEvents(true);
	RootComponent = BoxComponent;
}

void ADOTVolume::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		if (APlayerCharacter* PlayerInVolume = Cast<APlayerCharacter>(OtherActor)) {
			bStartDamage = true;
			playerInVolume = PlayerInVolume;
		}
	}
}

void ADOTVolume::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor) {
		if (APlayerCharacter* PlayerInVolume = Cast<APlayerCharacter>(OtherActor)) {
			bStartDamage = false;
			playerInVolume = nullptr;
		}
	}
}

// Called when the game starts or when spawned
void ADOTVolume::BeginPlay()
{
	Super::BeginPlay();
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ADOTVolume::OnBoxBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ADOTVolume::OnBoxEndOverlap);
}

// Called every frame
void ADOTVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (bStartDamage && playerInVolume && CurrentTime - StartTakingDamageTime >= 0.5f) {
		playerInVolume->TakeDamage(DamageAmount, FDamageEvent(), nullptr, this);
		StartTakingDamageTime = GetWorld()->GetTimeSeconds();
	}
}


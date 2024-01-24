// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnArea.h"
#include "Components/BoxComponent.h"
#include "OneShotGameModeBase.h"
#include "TutorialGameModeBase.h"


// Sets default values
ASpawnArea::ASpawnArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpawnBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnBox"));
	RootComponent = SpawnBox;
	
}

// Called when the game starts or when spawned
void ASpawnArea::BeginPlay()
{
	Super::BeginPlay();

	AOneShotGameModeBase* GM = Cast<AOneShotGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GM) {
		GM->SpawnArea = this;
	}
	
	ATutorialGameModeBase* TGM = Cast<ATutorialGameModeBase>(GetWorld()->GetAuthGameMode());
	if (bIsWeaponSpawnLocation && TGM) {
		TGM->WeaponSpawnAreas.Add(this);
	}
	else if (TGM) {
		TGM->SpawnArea = this;
	}
}

// Called every frame
void ASpawnArea::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


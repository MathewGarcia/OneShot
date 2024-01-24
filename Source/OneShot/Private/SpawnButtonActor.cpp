// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnButtonActor.h"
#include "TutorialGameModeBase.h"

// Sets default values
ASpawnButtonActor::ASpawnButtonActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ASpawnButtonActor::OnBoxBeginOverlap);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ASpawnButtonActor::OnBoxEndOverlap);
	RootComponent = BoxComponent;

}

void ASpawnButtonActor::SpawnEnemy()
{
	if (GM) {
		GM->SpawnEnemy();
	}
}

// Called when the game starts or when spawned
void ASpawnButtonActor::BeginPlay()
{
	Super::BeginPlay();

	GM = Cast<ATutorialGameModeBase>(GetWorld()->GetAuthGameMode());
	APCC = Cast<APlayerCharacterController>(GetWorld()->GetFirstPlayerController());
}

void ASpawnButtonActor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor) {
		if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor)) {
			player->bCanInteract = true;
			player->SpawnButtonActor = this;
			if (APCC) {
				APCC->SetTutorialText(FText::FromString("Press E to Spawn Enemy"));
			}
		}
	}
}

void ASpawnButtonActor::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor) {
		if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor)) {
			player->bCanInteract = false;
			player->SpawnButtonActor = nullptr;
			if (APCC) {
				if (GM) {
					GM->SetTutorialState(GM->GetTutorialState());
				}
			}
		}
	}
}


// Called every frame
void ASpawnButtonActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


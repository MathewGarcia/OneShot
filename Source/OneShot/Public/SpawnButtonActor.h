// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnButtonActor.generated.h"
class UBoxComponent;
class ATutorialGameModeBase;
class APlayerCharacterController;
class ASpawnArea;
UCLASS()
class ONESHOT_API ASpawnButtonActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnButtonActor();

	UPROPERTY(EditAnywhere, Category = "BoxComponent")
	UBoxComponent* BoxComponent;

	void SpawnEnemy();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	ATutorialGameModeBase* GM;

	APlayerCharacterController* APCC;
};

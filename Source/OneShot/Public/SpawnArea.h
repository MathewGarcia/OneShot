// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnArea.generated.h"

class UBoxComponent;
UCLASS()
class ONESHOT_API ASpawnArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnArea();

	UPROPERTY(EditAnywhere, Category = "SpawnArea")
	UBoxComponent* SpawnBox;

	UPROPERTY(EditAnywhere, Category = "WeaponToCheck")
	TSubclassOf<AWeapon>WeaponToCheck;

	AWeapon* CurrentSpawnedWeapon = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, Category = "IsWeaponSpawner")
	bool bIsWeaponSpawnLocation = false;

};

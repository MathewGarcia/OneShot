// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DOTVolume.generated.h"

class UBoxComponent;
UCLASS()
class ONESHOT_API ADOTVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADOTVolume();

	UPROPERTY(EditAnywhere, Category = "BoxComponent")
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, Category = "BoxComponent")
	float DamageAmount;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);


	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool bStartDamage;

	float StartTakingDamageTime = 0;

	APlayerCharacter* playerInVolume;
};

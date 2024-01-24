// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../../../../../../../Program Files/Epic Games/UE_5.3/Engine/Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputLibrary.h"
#include "HitByProjectileInterface.h"
#include "TutorialGameModeBase.h"
#include "PlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class EPlayerStates : uint8 {
	NONE UMETA(DisplayName = "NONE"),
	Sprint UMETA(DisplayName = "Sprint"),
	Stand UMETA(DisplayName = "Stand"),
	Crouch UMETA(DisplayName = "Crouch"),
	Slide UMETA(DisplayName = "Slide")
};

class AWeapon;
class AOneShotGameModeBase;
class APlayerCharacterController;
class ATutorialGameModeBase;
class ASpawnButtonActor;
class UNiagaraSystem;

UCLASS()
class ONESHOT_API APlayerCharacter : public ACharacter, public IHitByProjectileInterface
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	APlayerCharacter();

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputMappingContext* InputMapping;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
	class UInputConfigData* InputActions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Head camera")
	class UCameraComponent* HeadCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Sensitivity")
	USkeletalMeshComponent*FPSMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide")
	float MaxSlideTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide")
	float SlideSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide")
	float SlideRateToZero;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Sensitivity")
	float MouseSens = 0.3;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mouse Sensitivity")
	USoundBase*PickupSound;
	
	UPROPERTY(EditAnywhere, Category = "Blood Splatter")
	 UNiagaraSystem* BloodSplatter;

	FVector slideVel;

	void StartCrouch();

	void SetSlide();

	void Slide(float DeltaTime);

	void EndSlide();

	void Sprint();

	void Fire();

	void SetHealth(float NewHealth);

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TSubclassOf<AWeapon> WeaponToSpawnWith;

	UPROPERTY(EditAnywhere, Category = "DamageType")
	TSubclassOf<UDamageType> DamageType;

	UFUNCTION(Blueprintcallable, Category = "Weapon")
	AWeapon* GetCurrentWeapon();

	UFUNCTION(Blueprintcallable, Category = "Weapon")
	AWeapon* GetDefaultWeapon();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	FTimerHandle FFireRateTimerHandle;

	bool CanFire();

	void EquipWeapon(AWeapon*NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "PlayerMovement")
	EPlayerStates GetCurrentMovementState();


	void OpenMenu(const FInputActionValue& Value);

	void SetSensitivity(float newSensitivity);

	void PlayerJump();

	bool bCanInteract = false;

	void Interact();

	ASpawnButtonActor* SpawnButtonActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void HitByProjectile(float DamageAmount,AActor* Shooter, FDamageEvent& DamageEvent) override;
private:
	EPlayerStates playerMovementStates;

	FVector LastInput;

	float StartSlideTime;

	float WalkSpeed = 600.f;

	AWeapon* CurrentWeapon;

	AWeapon* DefaultWeapon;

	float MaxHealth = 100.f;
	AOneShotGameModeBase* GM;

	APlayerCharacterController* APCC;

	UFPSArmsAnimInstance* AnimInstance;

	ATutorialGameModeBase* TGM;

	FTutorialEventDelegate* TutorialDelegate = nullptr;

	bool bCanFire = true;

	FVector OriginalArmLocation;

};

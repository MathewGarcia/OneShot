// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "InputConfigData.h"
#include "SocketSubsystem.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Enemy.h"
#include "GameFramework/DamageType.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "Projectile.h"
#include "OneShotGameModeBase.h"
#include "ObjectPool.h"
#include "Components/SceneComponent.h"
#include "PlayerCharacterController.h"
#include "Kismet/GameplayStatics.h"
#include "FPSArmsAnimInstance.h"
#include <Components/SlateWrapperTypes.h>
#include "SpawnButtonActor.h"
#include "SpawnArea.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HeadCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("HeadCamera"));
	HeadCamera->SetupAttachment(RootComponent);
	HeadCamera->bUsePawnControlRotation = true;

	FPSMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPS Mesh"));
	FPSMesh->SetupAttachment(HeadCamera);
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void APlayerCharacter::StartCrouch()
{
	//more movement state logic
	switch (playerMovementStates)
	{
	case EPlayerStates::NONE:
	case EPlayerStates::Sprint:
		if (GetVelocity().Size() > 1000.0f)
		{
			SetSlide();
		}
		else {
			Crouch();
			playerMovementStates = EPlayerStates::Crouch;
			if (TutorialDelegate) {
				if (TGM && TGM->GetTutorialState() == ETutorialState::CROUCH) {
					TutorialDelegate->Broadcast(ETutorialState::JUMP);
				}
			}
		}
		break;
	case EPlayerStates::Crouch:
		UnCrouch();
		playerMovementStates = EPlayerStates::NONE;

		break;
	case EPlayerStates::Slide:
		break;
	default:
		break;
	}

}

void APlayerCharacter::SetSlide()
{
	UE_LOG(LogTemp, Warning, TEXT("Sliding"));

	//get the velocity we are currently at
	slideVel = GetVelocity();
	PreviousState = playerMovementStates;
		//set our player to sliding state
		playerMovementStates = EPlayerStates::Slide;
		//start the timer
		StartSlideTime = GetWorld()->GetTimeSeconds();
	
		HeadCamera->SetRelativeLocation(FVector(0.f,0.f,CrouchedEyeHeight));

		if (TutorialDelegate) {
			if (TGM && TGM->GetTutorialState() == ETutorialState::SLIDE) {
				TutorialDelegate->Broadcast(ETutorialState::CROUCH);
			}
		}
}

//comeback to this so we can maybe minimize the amount of left or right.
void APlayerCharacter::Slide(float DeltaTime)
{

	//if our magnitude is greater than the engines small number
	if (slideVel.SizeSquared() > KINDA_SMALL_NUMBER) {
		//declaring our vectors
		FVector ForwardDirection = GetActorForwardVector();
		FVector RightDirection = GetActorRightVector();
		//dot producting so we get the magnitude of these 2 vectors. We want these so that we can determine how much leeway to move left or right.
		float ForwardSpeed = FVector::DotProduct(slideVel, ForwardDirection);
		float RightSpeed = FVector::DotProduct(slideVel, RightDirection);

		//we get direction we want to move towards
		FVector SlideDirectionalMovement = ForwardDirection * ForwardSpeed * SlideSpeedMultiplier * DeltaTime;
		//we can still move left or right but its hard
		SlideDirectionalMovement += RightDirection * RightSpeed;
		//call it in add movement input
		AddMovementInput(SlideDirectionalMovement.GetSafeNormal(), SlideSpeedMultiplier * DeltaTime);
		//gradually lower our slide velocity until it reaches 0.
		slideVel = FMath::VInterpTo(slideVel, FVector::ZeroVector, DeltaTime, SlideRateToZero);

	}
	else if (!GetCharacterMovement()->IsMovingOnGround()) {
		EndSlide();
	}
	else {
		EndSlide();
	}

}

void APlayerCharacter::EndSlide()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	playerMovementStates = EPlayerStates::NONE;
	HeadCamera->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight));

}

void APlayerCharacter::Sprint()
{
	//just movement states
	if (GetCharacterMovement() && playerMovementStates != EPlayerStates::Slide) {
		switch (playerMovementStates) {
		case EPlayerStates::NONE:
		case EPlayerStates::Crouch:
			if (GetCharacterMovement()->IsCrouching()) {
				UnCrouch();
			}
				playerMovementStates = EPlayerStates::Sprint;
				GetCharacterMovement()->MaxWalkSpeed = 1200;
			break;
		case EPlayerStates::Sprint:
				playerMovementStates = EPlayerStates::NONE;
				GetCharacterMovement()->MaxWalkSpeed = 600;
			break;
		}
		if (TutorialDelegate) {
			if (TGM && TGM->GetTutorialState() == ETutorialState::SPRINT) {
				FTimerHandle Timerhandle;
				GetWorld()->GetTimerManager().SetTimer(Timerhandle, [this]() {
					TutorialDelegate->Broadcast(ETutorialState::SLIDE);
					}, 1.0f, false);
			}
		}
	}


}

void APlayerCharacter::Fire()
{
	if (!CanFire()) return;

	if (CurrentWeapon && CanFire()) {
		bCanFire = false;

		if (CurrentWeapon->FiringSound) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentWeapon->FiringSound, GetActorLocation());
		}
		if (CurrentWeapon->WeaponParticle) {
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CurrentWeapon->WeaponParticle, CurrentWeapon->WeaponMesh->GetSocketLocation("MuzzleFlashSocket"), CurrentWeapon->WeaponMesh->GetSocketRotation("MuzzleFlashSocket"), FVector(1.f), true, true, ENCPoolMethod::AutoRelease);
			UE_LOG(LogTemp, Warning, TEXT("Activated Emitter"));
		}
		if (AnimInstance) {
			AnimInstance->bIsFiring = true;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("AnimInstance failed"));
		}
		if (playerMovementStates == EPlayerStates::Sprint) {
			playerMovementStates = EPlayerStates::NONE;
		}

		if (TutorialDelegate) {
			if (TGM && TGM->GetTutorialState() == ETutorialState::FIRE) {
				TutorialDelegate->Broadcast(ETutorialState::PICKUP);
			}
		}
		bool bShotProjectile = false;

		switch (CurrentWeapon->GetWeaponType()) {
		case EWeaponType::HITSCAN: {

			FHitResult Hit;
			FVector Start = HeadCamera->GetComponentLocation();
			FVector ForwardVector = HeadCamera->GetForwardVector();
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);
			if (!CurrentWeapon->bIsShotgun) {
				FVector End = Start + ForwardVector * 10000;

				if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, CollisionParams)) {
					if (Hit.bBlockingHit) {
						//if this is an enemy else the cast will fail.
						if (AEnemy* HitEnemy = Cast<AEnemy>(Hit.GetActor())) {
							//spawn blood splatter
							UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodSplatter, Hit.ImpactPoint, FRotator::ZeroRotator, FVector(1.f), true, true, ENCPoolMethod::AutoRelease);

							FPointDamageEvent PointDamageEvent(CurrentWeapon->DamageAmount ,Hit, HeadCamera->GetForwardVector(), DamageType);
							//take damage away from the enemy.
							HitEnemy->TakeDamage(CurrentWeapon->DamageAmount, PointDamageEvent, GetController(), this);
							UE_LOG(LogTemp, Warning, TEXT("HitEnemy"));
						}
						//if we shot the projectile we can only destroy the projectile if we have a nondefault weapon, otherwise our default weapon is too weak and it must be a hitscan weapon!
						if (CurrentWeapon != DefaultWeapon) {
							if (AProjectile* EnemyProjectile = Cast<AProjectile>(Hit.GetActor())) {
								bShotProjectile = true;
								EnemyProjectile->ShotByPlayer();
							}
						}
					}
				}
				DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.0f, 0, 1.0f);

			}
			else { //same thing as above but we loop it for a shotgun.
				bool alreadyIncremented = false;

				for (int i = 0; i < CurrentWeapon->Shells; i++) { 
					//choose a random location in a cone.
					FVector RandomDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ForwardVector, 2);
					FVector End = Start + RandomDirection * 2000;
					if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility,CollisionParams)) {
						if (Hit.bBlockingHit) {
							if (AEnemy* HitEnemy = Cast<AEnemy>(Hit.GetActor())) {
								UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodSplatter, Hit.ImpactPoint, FRotator::ZeroRotator, FVector(1.f), true, true, ENCPoolMethod::AutoRelease);

								FPointDamageEvent PointDamageEvent(CurrentWeapon->DamageAmount, Hit, HeadCamera->GetForwardVector(), DamageType);
								//take damage away from the enemy.
								HitEnemy->TakeDamage(CurrentWeapon->DamageAmount, PointDamageEvent, GetController(), this);
							}
							//if we shot the projectile we can only destroy the projectile if we have a nondefault weapon, otherwise our default weapon is too weak
							if (CurrentWeapon != DefaultWeapon && !alreadyIncremented) {
								if (AProjectile* EnemyProjectile = Cast<AProjectile>(Hit.GetActor())) {
									bShotProjectile = true;
									EnemyProjectile->ShotByPlayer();
									alreadyIncremented = true;
								}
							}
						}
					}
					DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.0f, 0, 1.0f);

				}
			}

			break;
		}

		case EWeaponType::PROJECTILE: {
			//spawn the projectile instead this will be used for rpgs 
			if (CurrentWeapon) {
				float ShotProjectileDamage = CurrentWeapon->DamageAmount;
				FVector SpawnLocation = HeadCamera->GetComponentLocation() + HeadCamera->GetComponentRotation().Vector() * 100;
				if (CurrentWeapon->isHorizontalRPG) {
					FVector RightVector = HeadCamera->GetRightVector();

					for (int i = 0; i < 4; i++) {
						float HorizontalDifference = (i - 2) * 80.f;
						FVector DifferenceSpawnLocation = SpawnLocation + RightVector * HorizontalDifference;
						//update this to spawn from the barrel of the weapon??
						AProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(CurrentWeapon->Projectile, DifferenceSpawnLocation, FRotator::ZeroRotator);
						SpawnedProjectile->PlayerWeaponDamage = ShotProjectileDamage;
						SpawnedProjectile->SetOwner(this);
						FVector Direction = HeadCamera->GetForwardVector();
						SpawnedProjectile->FireInDirection(Direction);
					}
				}
				else {
					//update this to spawn from the barrel of the weapon??
					AProjectile* SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(CurrentWeapon->Projectile, SpawnLocation, FRotator::ZeroRotator);
					SpawnedProjectile->PlayerWeaponDamage = ShotProjectileDamage;
					SpawnedProjectile->SetOwner(this);
					FVector Direction = HeadCamera->GetForwardVector();
					SpawnedProjectile->FireInDirection(Direction);
				}
			}
		}
			break;

		
		}

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
			if (AnimInstance) {
				AnimInstance->bIsFiring = false;
				}
			bCanFire = true;
			}, CurrentWeapon->fireRate, false);

		if (CurrentWeapon != DefaultWeapon && !bShotProjectile) {
				CurrentWeapon->CurrentAmmo--;
				if (CurrentWeapon->CurrentAmmo < 1) {
			EquipWeapon(DefaultWeapon);
				}
		}
		else if (CurrentWeapon == DefaultWeapon) {
			GetWorld()->GetTimerManager().SetTimer(FFireRateTimerHandle, CurrentWeapon->fireRate, false);
		}
		else {
			if (TutorialDelegate) {
				if (TGM && TGM->GetTutorialState() == ETutorialState::FINAL) {
					TutorialDelegate->Broadcast(ETutorialState::NONE);
				}
			}
		}
	}

}

void APlayerCharacter::SetHealth(float NewHealth)
{
	Health = NewHealth;
	if (APCC) {
		APCC->SetHealth(Health);
	}
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	//spawn the weapon
	AWeapon*SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponToSpawnWith, GetActorLocation(), GetActorRotation());
	//Equip it
	EquipWeapon(SpawnedWeapon);
	//set the default weapon
	DefaultWeapon = SpawnedWeapon;
	TGM = Cast<ATutorialGameModeBase>(GetWorld()->GetAuthGameMode());
	if (TGM) {
		TutorialDelegate = &TGM->OnTutorialEvent;
	}
	GM = Cast<AOneShotGameModeBase>(GetWorld()->GetAuthGameMode());
	APCC = Cast<APlayerCharacterController>(GetController());
	AnimInstance = Cast<UFPSArmsAnimInstance>(FPSMesh->GetAnimInstance());
	SetHealth(MaxHealth);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//tick the slide
	if (playerMovementStates == EPlayerStates::Slide) {		
		//we slide
		Slide(DeltaTime);
		//if we reach our max slide time, end the slide
		if (GetWorld()->GetTimeSeconds() - StartSlideTime >= MaxSlideTime) {
			EndSlide();
		}
	}
}

void APlayerCharacter::HitByProjectile(float DamageAmount,AActor* Shooter,FDamageEvent&DamageEvent)
{
	if (AEnemy* EnemyShooter = Cast<AEnemy>(Shooter)) {
		TakeDamage(DamageAmount, DamageEvent, EnemyShooter->GetController(), EnemyShooter);
	}
}

// Called to bind functionality to input

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	PEI->BindAction(InputActions->InputCrouch, ETriggerEvent::Triggered, this, &APlayerCharacter::StartCrouch);
	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Triggered, this, &APlayerCharacter::Sprint);
	PEI->BindAction(InputActions->InputFire, ETriggerEvent::Triggered, this, &APlayerCharacter::Fire);
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Triggered, this, &APlayerCharacter::PlayerJump);
	PEI->BindAction(InputActions->InputMenu, ETriggerEvent::Triggered, this, &APlayerCharacter::OpenMenu);
	PEI->BindAction(InputActions->InputInteraction, ETriggerEvent::Triggered, this , &APlayerCharacter::Interact);
}

void APlayerCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor) {
		//if we walk over a weapon, equip the weapon
		if (AWeapon* Weapon = Cast<AWeapon>(OtherActor)) {
			EquipWeapon(Weapon);
			if (TutorialDelegate) {
				if (TGM && TGM->GetTutorialState() == ETutorialState::PICKUP) {
					TutorialDelegate->Broadcast(ETutorialState::FINAL);
				}
			}
		}
	}
}


void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

		for (FTimerHandle& TimerHandle : WeaponRespawnTimerHandles) {

			UE_LOG(LogTemp, Warning, TEXT("Clearing weapon respawn timers"));
			GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

		}
		WeaponRespawnTimerHandles.Empty();

		if (GM) {
			GM->EndGame();
		}
	
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

AWeapon* APlayerCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

AWeapon* APlayerCharacter::GetDefaultWeapon()
{
	return DefaultWeapon;
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//if we are in the tutorial area, dont take damage.
	if (!GM) { return 0; }
	//set the health
	SetHealth(Health - DamageAmount);

	if (APCC) {
		APCC->SetHitOverlayVisibility(ESlateVisibility::Visible);
		FTimerHandle FDisableHitVisibilyHandle;
		GetWorld()->GetTimerManager().SetTimer(FDisableHitVisibilyHandle, [this]() {
			APCC->SetHitOverlayVisibility(ESlateVisibility::Collapsed);
			},
		0.05, false);
	}
	//not implemented yet but if we die kill the player (change scene to game over screen) showing the survival time and score achieved
	if (Health <= 0.f) {
		float DeathTime = GetWorld()->GetTimeSeconds();
		int Min = FMath::FloorToInt(DeathTime / 60.f);
		int Sec = FMath::RoundToInt(DeathTime) % 60;

		if (APCC) {
			APCC->SetDeathWidgetVisibility(ESlateVisibility::Visible);
			if (GM) {
				int deathScore = GM->PlayerScore;
				FString deathString = FString::Printf(TEXT("YOU DIED WITH A HIT OF %f YOU LASTED %02d:%02d with %d points"),DamageAmount ,Min, Sec, deathScore);
				FText text = FText::FromString(deathString);
				APCC->SetDeathText(text);
			}
		}

		Die();
	}

	return DamageAmount;
}

bool APlayerCharacter::CanFire()
{
	//if we can fire and the current ammo is greater than 0 and we don't have the fire rate timer active then we can fire.
	return CurrentWeapon && CurrentWeapon->CurrentAmmo > 0 && !GetWorld()->GetTimerManager().IsTimerActive(FFireRateTimerHandle) && bCanFire;
}

void APlayerCharacter::EquipWeapon(AWeapon* NewWeapon)
{
	if (!NewWeapon)  return;

	if (PickupSound) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound,GetActorLocation());
	}
	if (NewWeapon->WeaponDrop) {
		NewWeapon->WeaponDrop->Deactivate();
	}
	//if we have a current weapon and a default weapon and the new weapon we are trying to equip is not equal to the default weapon
	if (CurrentWeapon && DefaultWeapon) {
		//detatch the weapon from the player
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		//Send used weapon to the object pool.
		GM->WeaponPool->ReturnPooledObject(CurrentWeapon);
		//we also want to hide our default weapon
		DefaultWeapon->SetActorHiddenInGame(true);
	}

	//set our current weapon to the new weapon
	CurrentWeapon = NewWeapon;

	//if the current weapon is equal to our default weapon
	if (CurrentWeapon == DefaultWeapon) {
		//hide it. We do this because since our object pool does this natievly. We need to do this ourselves since the default weapon is not in the object pool
		CurrentWeapon->SetActorHiddenInGame(false);
	}

	//attach weapon to player
	NewWeapon->AttachToComponent(FPSMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponSocket");
	//change the weapon actors collision to no collision so that the player doesnt collide with the weapon when holding it.
	if (NewWeapon->WeaponMesh) {
		NewWeapon->WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	//set the current weapon count to subtract because we can only shoot a picked up weapon once. Once we do, we tell the game mode to spawn a new weapon in 15 seconds after picking up the weapon.
	if (GM) {
		GM->SetCurrentWeaponAmount(GM->GetCurrentWeaponAmount() - 1);
		if (GM->GetCurrentWeaponAmount() < GM->MaxWeaponsAllowed) {
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
				GM->SetWeapon();
				},15.f, false);

			WeaponRespawnTimerHandles.Add(TimerHandle);

		}

	}
	else if (TGM) {
		for (int i = 0; i < TGM->WeaponSpawnAreas.Num(); i++){
			if (TGM->WeaponSpawnAreas[i]->CurrentSpawnedWeapon && TGM->WeaponSpawnAreas[i]->CurrentSpawnedWeapon == CurrentWeapon)
			{
				TGM->WeaponSpawnAreas[i]->CurrentSpawnedWeapon = nullptr;
				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this,i]() {
					TGM->SpawnWeapon(TGM->WeaponSpawnAreas[i]);
					}, 1.0f, false);
				WeaponRespawnTimerHandles.Add(TimerHandle);

			}

		}
	}
}

EPlayerStates APlayerCharacter::GetCurrentMovementState()
{
	return playerMovementStates;
}

void APlayerCharacter::OpenMenu(const FInputActionValue& Value)
{

	//check if the game is paused
	bool bIsPaused = UGameplayStatics::IsGamePaused(GetWorld());
	//create a new pause state opposite of the current one
	bool bNewPauseState = !bIsPaused;
	if (APCC) {
		if (bNewPauseState) {
			//pausing
			APCC->SetMainMenuVisibility(ESlateVisibility::Visible);
			APCC->SetInputMode(FInputModeGameAndUI());
			APCC->FlushPressedKeys();
			APCC->SetShowMouseCursor(true);


		}
		else {
			//unpause
			APCC->SetMainMenuVisibility(ESlateVisibility::Collapsed);
			APCC->SetShowMouseCursor(false);
			APCC->SetInputMode(FInputModeGameOnly());
		}
		UGameplayStatics::SetGamePaused(GetWorld(), bNewPauseState);
	}
}

void APlayerCharacter::SetSensitivity(float newSensitivity)
{
	MouseSens = newSensitivity;
}

void APlayerCharacter::PlayerJump()
{
	Jump();
	if (TutorialDelegate) {
		if (TGM && TGM->GetTutorialState() == ETutorialState::JUMP) {
			TutorialDelegate->Broadcast(ETutorialState::FIRE);
		}
	}
}

void APlayerCharacter::Interact()
{
	if (bCanInteract && SpawnButtonActor) {
		SpawnButtonActor->SpawnEnemy();
	}
}

void APlayerCharacter::Die()
{
	//death
	GetCharacterMovement()->DisableMovement();
	FRotator DeathRotation = FRotator(0, 0, 0);
	FRotator CurrentRotation = GetActorRotation();
	float LerpSpeed = 0.5f;
	FRotator NewRotation = FMath::Lerp(CurrentRotation, DeathRotation, LerpSpeed);
	SetActorRotation(NewRotation);

	EndPlay(EEndPlayReason::LevelTransition);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{

	if (GetController() != nullptr) {
		//get the input key and make a vector out of it (-1,1,etc)
		const FVector2D MoveValue = Value.Get<FVector2D>();
		//create a movement rotation based on the direction the camera is looking
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);

		if (MoveValue.Y != 0.0f) {
			//move forward fromt he camera direction
			const FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(Direction, MoveValue.Y);
		}
		if (MoveValue.X != 0.f) {
			//move right of the camera direction
			const FVector Direction = MovementRotation.RotateVector(FVector::RightVector);
			AddMovementInput(Direction, MoveValue.X);
		}

		if (TutorialDelegate) {
			if (TGM && TGM->GetTutorialState() == ETutorialState::MOVEMENT) {
				TutorialDelegate->Broadcast(ETutorialState::LOOK);
			}
		}
	}

}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	if (GetController() != nullptr)
	{
		//get the input and create a vector out of it (-1,1,etc)
		const FVector2D LookValue = Value.Get<FVector2D>();

		if (LookValue.X != 0.f)
		{
			//move the camera to that direction and account for the mouse sensitivity
			AddControllerYawInput(LookValue.X*MouseSens);
		}

		if (LookValue.Y != 0.f)
		{
			AddControllerPitchInput(-LookValue.Y*MouseSens);
		}

		if (TutorialDelegate) {
			if (TGM && TGM->GetTutorialState() == ETutorialState::LOOK) {
				FTimerHandle Timerhandle;
				GetWorld()->GetTimerManager().SetTimer(Timerhandle, [this]() {
					TutorialDelegate->Broadcast(ETutorialState::SPRINT);
					}, 1.0f, false);
			}
		}
	}
}


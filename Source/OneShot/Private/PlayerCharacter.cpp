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

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HeadCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("HeadCamera"));
	HeadCamera->SetupAttachment(RootComponent);
	HeadCamera->bUsePawnControlRotation = true;

	GetMesh()->SetupAttachment(HeadCamera);
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
}

void APlayerCharacter::StartCrouch()
{
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

		//set our player to sliding state
		playerMovementStates = EPlayerStates::Slide;
		//start the timer
		StartSlideTime = GetWorld()->GetTimeSeconds();
	
		HeadCamera->SetRelativeLocation(FVector(0.f,0.f,CrouchedEyeHeight));
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
	if (GetCharacterMovement()) {
		switch (playerMovementStates) {
		case EPlayerStates::NONE:
		case EPlayerStates::Crouch:
			playerMovementStates = EPlayerStates::Sprint;
			GetCharacterMovement()->MaxWalkSpeed *= 2;
			break;
		case EPlayerStates::Sprint:
			playerMovementStates = EPlayerStates::NONE;
			GetCharacterMovement()->MaxWalkSpeed /= 2;
			break;
		}
	}


}

void APlayerCharacter::Fire()
{
	if (CurrentWeapon) {
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
							FPointDamageEvent PointDamageEvent(CurrentWeapon->DamageAmount ,Hit, HeadCamera->GetForwardVector(), DamageType);
							//take damage away from the enemy.
							HitEnemy->TakeDamage(CurrentWeapon->DamageAmount, PointDamageEvent, GetController(), this);
							UE_LOG(LogTemp, Warning, TEXT("HitEnemy"));
						}
					}
				}
				DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.0f, 0, 1.0f);

			}
			else { //same thing as above but we loop it for a shotgun.
				for (int i = 0; i < CurrentWeapon->Shells; i++) { 
					//choose a random location in a cone.
					FVector RandomDirection = UKismetMathLibrary::RandomUnitVectorInConeInDegrees(ForwardVector, 2);
					FVector End = Start + RandomDirection * 1000;

					if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility,CollisionParams)) {
						if (Hit.bBlockingHit) {
							if (AEnemy* HitEnemy = Cast<AEnemy>(Hit.GetActor())) {
								FPointDamageEvent PointDamageEvent(CurrentWeapon->DamageAmount, Hit, HeadCamera->GetForwardVector(), DamageType);
								//take damage away from the enemy.
								HitEnemy->TakeDamage(CurrentWeapon->DamageAmount, PointDamageEvent, GetController(), this);
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
				//update this to spawn from the barrel of the weapon??
				AProjectile*SpawnedProjectile = GetWorld()->SpawnActor<AProjectile>(CurrentWeapon->Projectile);
				SpawnedProjectile->SetOwner(this);
				FVector Direction = HeadCamera->GetComponentLocation() + HeadCamera->GetForwardVector();
				SpawnedProjectile->FireInDirection(Direction);
			}
		}
			break;

		
		}
	}
}

void APlayerCharacter::SetHealth(float NewHealth)
{
	Health = NewHealth;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	AWeapon*SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponToSpawnWith, GetActorLocation(), GetActorRotation());
	EquipWeapon(SpawnedWeapon);
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
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Triggered, this, &APlayerCharacter::Jump);

}

void APlayerCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (OtherActor) {
		if (AWeapon* Weapon = Cast<AWeapon>(OtherActor)) {
			EquipWeapon(Weapon);
		}
	}
}

AWeapon* APlayerCharacter::GetCurrentWeapon()
{
	return CurrentWeapon;
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

	SetHealth(Health - DamageAmount);
	UE_LOG(LogTemp, Warning, TEXT("Player hit with %f Damage"),DamageAmount);

	if (Health <= 0.f) {
		UE_LOG(LogTemp, Warning, TEXT("Player is dead"));
	}

	return DamageAmount;
}

void APlayerCharacter::EquipWeapon(AWeapon* NewWeapon)
{
	CurrentWeapon = NewWeapon;
	//attach weapon to player
	NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
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
			//move the camera to that direction
			AddControllerYawInput(LookValue.X);
		}

		if (LookValue.Y != 0.f)
		{
			AddControllerPitchInput(-LookValue.Y);
		}
	}
}


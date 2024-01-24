// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterController.h"
#include "PlayerUIWidget.h"

void APlayerCharacterController::SetScore(int newScore)
{
	if (PlayerUIWidget) {
		PlayerUIWidget->SetScore(newScore);
	}
}

void APlayerCharacterController::SetHealth(int newHealth)
{
	if (PlayerUIWidget) {
		PlayerUIWidget->SetHealth(newHealth);
	}
}

void APlayerCharacterController::SetMainMenuVisibility(ESlateVisibility Visibile)
{
	if (PlayerUIWidget) {
		PlayerUIWidget->SetMainMenuVisibility(Visibile);
	}
}

void APlayerCharacterController::SetTutorialText(FText Text)
{
	if (PlayerUIWidget) {
		PlayerUIWidget->SetTutorialText(Text);
	}
}

void APlayerCharacterController::SetTutorialVisibility(ESlateVisibility VisibilityType)
{
	if (PlayerUIWidget) {
		PlayerUIWidget->SetTutorialVisibility(VisibilityType);
	}
}

void APlayerCharacterController::SetDeathWidgetVisibility(ESlateVisibility VisibilityType)
{
	if (PlayerUIWidget) {
		PlayerUIWidget->SetDeathWidgetVisibility(VisibilityType);
	}
}

void APlayerCharacterController::SetDeathText(FText Text)
{
	if (PlayerUIWidget) {
		PlayerUIWidget->SetDeathText(Text);
	}
}

void APlayerCharacterController::SetHitOverlayVisibility(ESlateVisibility VisibilityType)
{
	if (PlayerUIWidget) {
		PlayerUIWidget->SetHitOverlayVisibility(VisibilityType);
	}

}

void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameOnly());
	if (PlayerUIWidgetClass) {
		 PlayerUIWidget = CreateWidget<UPlayerUIWidget>(this, PlayerUIWidgetClass);
		if (PlayerUIWidget) {
			PlayerUIWidget->AddToViewport();
			SetMainMenuVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void APlayerCharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerUIWidget) {
		PlayerUIWidget->SetTime(GetWorld()->GetTimeSeconds());
	}
}

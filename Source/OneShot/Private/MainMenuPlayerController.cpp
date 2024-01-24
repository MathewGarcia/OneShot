// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMainMenuPlayerController::BeginPlay()
{
	SetInputMode(FInputModeUIOnly());
	if (WidgetUI) {
		UUserWidget* Widget = CreateWidget<UUserWidget>(this,WidgetUI);
		if (Widget) {
			Widget->AddToViewport();
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("WidgetUI failed"));
	}
}

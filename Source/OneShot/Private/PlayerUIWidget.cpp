// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerUIWidget.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Components/EditableTextBox.h"
#include "PlayerCharacter.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerInput.h"


void UPlayerUIWidget::EditBox(const FText& text)
{
	UE_LOG(LogTemp, Warning, TEXT("Editing Box"));
	FString incomingText = text.ToString();
	if (FCString::IsNumeric(*incomingText) && FCString::Atof(*incomingText) >= 0.0f) {
		if (APlayerCharacter* player = Cast<APlayerCharacter>(GetOwningPlayerPawn())) {
			player->SetSensitivity(FCString::Atof(*incomingText));
			PlayerSens = player->MouseSens;
		}
	}
	else {
		FString validText = FString::SanitizeFloat(PlayerSens);
		SensitivityBox->SetText(FText::FromString(validText));
	}
}

void UPlayerUIWidget::SetScore(int newScore)
{
	FString ScoreString = FString::Printf(TEXT("SCORE: %d"), newScore);
	ScoreTextBlock->SetText(FText::FromString(ScoreString));
}

void UPlayerUIWidget::SetTime(float currentTime)
{
	if (SurvivalTimeTextBlock) {
		int Min = FMath::FloorToInt(currentTime / 60.f);
		int Sec = FMath::RoundToInt(currentTime) % 60;
		FString time = FString::Printf(TEXT("%02d:%02d"), Min, Sec);
		SurvivalTimeTextBlock->SetText(FText::FromString(time));
	}
}

void UPlayerUIWidget::SetHealth(int newHealth)
{
	FString HealthString = FString::Printf(TEXT("%d"), newHealth);
	HealthTextBlock->SetText(FText::FromString(HealthString));
}

void UPlayerUIWidget::SetTutorialText(FText Text)
{
	TutorialTextBlock->SetText(Text);
}

void UPlayerUIWidget::SetTutorialVisibility(ESlateVisibility VisibilityType)
{
	TutorialOverlay->SetVisibility(VisibilityType);
}

void UPlayerUIWidget::SetMainMenuVisibility(ESlateVisibility VisibilityType)
{
	MainMenuOverlay->SetVisibility(VisibilityType);
}

void UPlayerUIWidget::SetDeathWidgetVisibility(ESlateVisibility VisibilityType)
{
	DeathOverlay->SetVisibility(VisibilityType);
}

void UPlayerUIWidget::SetDeathText(FText Text)
{
	DeathText->SetText(Text);
}

void UPlayerUIWidget::SetHitOverlayVisibility(ESlateVisibility VisibilityType)
{
	HitOverlay->SetVisibility(VisibilityType);
}

void UPlayerUIWidget::NativeConstruct()
{
	MainMenuOverlay->SetVisibility(ESlateVisibility::Collapsed);
	SensitivityBox->OnTextChanged.AddDynamic(this, &UPlayerUIWidget::EditBox);

	if (APlayerCharacter*player= Cast<APlayerCharacter>(GetOwningPlayerPawn())) {
		PlayerSens = player->MouseSens;
	}
	if (ATutorialGameModeBase* GM = Cast<ATutorialGameModeBase>(GetWorld()->GetAuthGameMode())) {
		SetTutorialText(FText::FromString("Use W,A,S,D to move"));
	}

}

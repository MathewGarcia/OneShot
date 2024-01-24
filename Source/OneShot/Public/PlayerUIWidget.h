// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUIWidget.generated.h"

/**
 * 
 */
class UTextBlock;
class UOverlay;
class UEditableTextBox;
UCLASS(Blueprintable)
class ONESHOT_API UPlayerUIWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SurvivalTimeTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthTextBlock;

	UPROPERTY(meta = (BindWidget))
	UOverlay* MainMenuOverlay;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* SensitivityBox;

	UPROPERTY(meta = (BindWidget))
	UOverlay* TutorialOverlay;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TutorialTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock*DeathText;

	UPROPERTY(meta = (BindWidget))
	UOverlay*DeathOverlay;

	UPROPERTY(meta = (BindWidget))
	UOverlay*HitOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Sensitivity")
	float PlayerSens;

	UFUNCTION(BlueprintCallable, Category = "Setting Sensitivity")
	void EditBox(const FText& text);

	void SetScore(int newScore);

	void SetTime(float currentTime);

	void SetHealth(int newHealth);

	void SetTutorialText(FText Text);

	void SetTutorialVisibility(ESlateVisibility VisibilityType);

	void SetMainMenuVisibility(ESlateVisibility VisibilityType);

	void SetDeathWidgetVisibility(ESlateVisibility VisibilityType);

	void SetDeathText(FText Text);

	void SetHitOverlayVisibility(ESlateVisibility VisibilityType);



protected:
	virtual void NativeConstruct() override;
};

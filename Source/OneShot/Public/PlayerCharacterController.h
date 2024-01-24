// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

/**
 * 
 */
class UPlayerUIWidget;
UCLASS()
class ONESHOT_API APlayerCharacterController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UPlayerUIWidget>PlayerUIWidgetClass;

	void SetScore(int newScore);

	void SetHealth(int newHealth);

	void SetMainMenuVisibility(ESlateVisibility Visibile);

	void SetTutorialText(FText Text);

	void SetTutorialVisibility(ESlateVisibility VisibilityType);

	void SetDeathWidgetVisibility(ESlateVisibility VisibilityType);

	void SetDeathText(FText Text);

	void SetHitOverlayVisibility(ESlateVisibility VisibilityType);

private:
	UPlayerUIWidget* PlayerUIWidget;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
};

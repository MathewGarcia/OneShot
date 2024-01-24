// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ONESHOT_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, Category = "PlayerUI")
	TSubclassOf<UUserWidget>WidgetUI;

private:

protected:
	virtual void BeginPlay() override;

};

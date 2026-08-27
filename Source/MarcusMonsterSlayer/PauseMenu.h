// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"

#include "PauseMenu.generated.h"

/**
 *
 */
UCLASS()
class MARCUSMONSTERSLAYER_API UPauseMenu : public UUserWidget
{
	GENERATED_BODY()
public: 

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* ResumeButton;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UButton* QuitButton;

protected:

	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnResumeClicked();

	UFUNCTION()
	void  OnQuitClicked();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class MARCUSMONSTERSLAYER_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* HPBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* CurrencyText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* HPText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* LevelText;

	void SetHP(int CurrentHP, int MaxHP);
	void SetCurrency(int Amount);
	void SetLevel(int Index);

};

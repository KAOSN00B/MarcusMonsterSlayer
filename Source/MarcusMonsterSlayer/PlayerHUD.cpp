// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

void UPlayerHUD::SetHP(int CurrentHP, int MaxHP)
{
	FString Str = FString::Printf(TEXT("HP:%d/%d"), CurrentHP, MaxHP);
	HPText->SetText(FText::FromString(Str));

	float Percent = (float)CurrentHP / (float)MaxHP;
	HPBar->SetPercent(Percent);
}

void UPlayerHUD::SetCurrency(int Amount)
{
	FString Str = FString::Printf(TEXT("Gold:%d"), Amount);
	CurrencyText->SetText(FText::FromString(Str));
}

void UPlayerHUD::SetLevel(int Index)
{
	FString Str = FString::Printf(TEXT("Level:%d"), Index);
	LevelText->SetText(FText::FromString(Str));
}

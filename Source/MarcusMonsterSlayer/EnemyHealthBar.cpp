// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyHealthBar.h"

void UEnemyHealthBar::SetHealth(int CurrentHP, int MaxHP)
{
	float Percent = (float)CurrentHP / (float)MaxHP;
	HealthBar->SetPercent(Percent);
}

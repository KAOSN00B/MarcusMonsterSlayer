// Fill out your copyright notice in the Description page of Project Settings.


#include "MarcusMonsterHunterGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UMarcusMonsterHunterGameInstance::SetPlayerHP(int NewCurrentHP, int MaxHP)
{
	PlayerHP = NewCurrentHP;
	PlayerMaxHP = MaxHP;

}

void UMarcusMonsterHunterGameInstance::AddMoney(int Amount)
{
	CollectedMoneyCount += Amount;
}

void UMarcusMonsterHunterGameInstance::ChangeLevel(TSoftObjectPtr<UWorld> NewLevel)
{
	NextLevel = NewLevel;
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, NextLevel);
}

void UMarcusMonsterHunterGameInstance::TriggerGameOverScreen()
{
}

void UMarcusMonsterHunterGameInstance::RestartGame()
{
	PlayerHP = PlayerMaxHP;
	CollectedMoneyCount = 0;
	IsDoubleJumpUnlocked = false;

	ChangeLevel(StartingLevel);
}

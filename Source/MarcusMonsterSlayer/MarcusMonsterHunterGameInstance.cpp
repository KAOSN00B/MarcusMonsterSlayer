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

void UMarcusMonsterHunterGameInstance::ChangeLevel(int LevelIndex)
{
	if (LevelIndex <= 0) return;

	CurrentLevelIndex = LevelIndex;
	FString LevelNameString = FString::Printf(TEXT("Level_%d"), LevelIndex);
	UGameplayStatics::OpenLevel(GetWorld(), FName(LevelNameString));

}

void UMarcusMonsterHunterGameInstance::TriggerGameOverScreen()
{
}

void UMarcusMonsterHunterGameInstance::RestartGame()
{
	PlayerHP = PlayerMaxHP;
	CollectedMoneyCount = 0;
	IsDoubleJumpUnlocked = false;


	CurrentLevelIndex = 1;
	ChangeLevel(CurrentLevelIndex);
}

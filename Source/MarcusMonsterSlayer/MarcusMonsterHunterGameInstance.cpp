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
	// checkpoints don't carry across level boundaries
	HasCheckpoint = false;

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

void UMarcusMonsterHunterGameInstance::SetCheckpoint(FName LevelName, FVector Location)
{
	HasCheckpoint = true;
	CheckpointLevelName = LevelName;
	CheckpointLocation = Location;
}

void UMarcusMonsterHunterGameInstance::RespawnAtCheckpoint()
{
	// no checkpoint reached yet - fall back to a full restart
	if (!HasCheckpoint)
	{
		RestartGame();
		return;
	}

	PlayerHP = PlayerMaxHP;                          // respawn at full health
	UGameplayStatics::OpenLevel(this, CheckpointLevelName);   // reload the level -> enemies reset
}

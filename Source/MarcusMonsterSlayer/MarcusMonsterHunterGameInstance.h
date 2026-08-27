// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MarcusMonsterHunterGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MARCUSMONSTERSLAYER_API UMarcusMonsterHunterGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int PlayerHP = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int PlayerMaxHP = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int CollectedMoneyCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> NextLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> StartingLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsDoubleJumpUnlocked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool HasInitializedHP = false;

	void SetPlayerHP(int NewCurrentHP, int MaxHP);
	void AddMoney(int Amount);

	void ChangeLevel(TSoftObjectPtr<UWorld> NewLevel);
	void TriggerGameOverScreen();
	void RestartGame();
};

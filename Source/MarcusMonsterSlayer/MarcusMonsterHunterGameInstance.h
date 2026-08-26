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
	int PlayerHP;
	int PlayerMaxHP;

	void SetPlayerHP(int NewCurrentHP, int MaxHP);
};

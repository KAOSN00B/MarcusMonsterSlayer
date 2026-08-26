// Fill out your copyright notice in the Description page of Project Settings.


#include "MarcusMonsterHunterGameInstance.h"

void UMarcusMonsterHunterGameInstance::SetPlayerHP(int NewCurrentHP, int MaxHP)
{
	PlayerHP = NewCurrentHP;
	PlayerMaxHP = MaxHP;
}

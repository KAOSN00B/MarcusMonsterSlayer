// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "MyTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class MARCUSMONSTERSLAYER_API AMyTriggerBox : public ATriggerBox
{
	GENERATED_BODY()
	
public:
    AMyTriggerBox();

protected:
    virtual void BeginPlay() override;

    // Must be UFUNCTION to bind with dynamic multicast delegates
    UFUNCTION()
    void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);
};

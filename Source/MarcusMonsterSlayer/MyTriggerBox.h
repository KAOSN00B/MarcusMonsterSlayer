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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int DamageToPlayer = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StunPlayerAmount = .3f;

	UPROPERTY(EditAnywhere)
	float ActiveDuration = 2.0f;

	UPROPERTY(EditAnywhere)
	float InactiveDuration = 1.5f;

	UPROPERTY(EditAnywhere)
	float StartDelay = 0.0f;

	UPROPERTY(EditAnywhere)
	float TeleportDelay = 0.5f;

	UPROPERTY(EditAnywhere)
	bool TeleportPlayerOnHit = false;

	UPROPERTY(EditAnywhere)
	bool DisappearsOnTimer = false;

	UPROPERTY()
	class AMarcus* PendingTeleportPlayer = nullptr;

	FTimerHandle TeleportDelayTimer;

	FTimerHandle CycleTimer;


	virtual void BeginPlay() override;

	// Must be UFUNCTION to bind with dynamic multicast delegates
	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	void TeleportPlayerToLastLocation(class AMarcus* Marcus);

	void ActivateHazard();
	void DeactiveHazard();
	void DoDelayedTeleport();
};

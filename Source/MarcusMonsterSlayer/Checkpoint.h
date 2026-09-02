// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

UCLASS()
class MARCUSMONSTERSLAYER_API ACheckpoint : public AActor
{
	GENERATED_BODY()

public:
	ACheckpoint();

protected:

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere)
	class UPaperFlipbookComponent* Flipbook;

	// shown before the player reaches it (unlit candle)
	UPROPERTY(EditAnywhere)
	class UPaperFlipbook* InactiveAnim;

	// shown once activated (lit candle)
	UPROPERTY(EditAnywhere)
	class UPaperFlipbook* ActiveAnim;

	UPROPERTY(EditAnywhere)
	class USoundBase* ActivateSound;

	bool bActivated = false;

	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};

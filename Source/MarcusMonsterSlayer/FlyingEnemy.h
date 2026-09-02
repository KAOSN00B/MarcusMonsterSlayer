// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlyingEnemy.generated.h"

UCLASS()
class MARCUSMONSTERSLAYER_API AFlyingEnemy : public AActor
{
	GENERATED_BODY()

public:
	AFlyingEnemy();
	virtual void Tick(float DeltaTime) override;

	// called by Marcus's attack box when the sword overlaps this enemy
	void TakeHit(int Damage);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* ContactBox;

	UPROPERTY(VisibleAnywhere)
	class UPaperFlipbookComponent* Flipbook;

	// one-way travel from the spawn point (world units). X = horizontal, Z = vertical, mix for diagonal.
	UPROPERTY(EditAnywhere)
	FVector PatrolOffset = FVector(300.0f, 0.0f, 0.0f);

	// full there-and-back trips per second (0.25 = one round trip every 4s)
	UPROPERTY(EditAnywhere)
	float PatrolSpeed = 0.25f;

	UPROPERTY(EditAnywhere)
	int MaxHitPoints = 1;

	UPROPERTY(EditAnywhere)
	int ContactDamage = 1;

	UPROPERTY(EditAnywhere)
	float ContactStunDuration = 0.3f;

	// mirror the sprite horizontally to face the way it's moving
	UPROPERTY(EditAnywhere)
	bool FaceTravelDirection = true;

	// plays once on death, then the actor is destroyed; leave empty to just vanish
	UPROPERTY(EditAnywhere)
	class UPaperFlipbook* DeathAnim;

	// multiplier on the sprite scale when the death anim plays (reused sprites are often authored at a different size)
	UPROPERTY(EditAnywhere)
	float DeathAnimScale = 1.0f;

	UPROPERTY(EditAnywhere)
	class USoundBase* DeathSound;

	UFUNCTION()
	void OnContactBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDeathAnimFinished();

private:
	FVector PatrolStart;
	FVector PatrolEnd;
	float PatrolTime = 0.0f;
	int HitPoints = 1;
	bool bDead = false;
};

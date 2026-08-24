// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"

#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"
#include "PaperZDAnimInstance.h"

#include "Engine/TimerHandle.h"
#include "GameFramework/Character.h"

#include "Marcus.h"
#include "Enemy.generated.h"

/**
 *
 */
UCLASS()
class MARCUSMONSTERSLAYER_API AEnemy : public APaperZDCharacter
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USphereComponent* PlayerDetectorSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTextRenderComponent* HPText;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* AttackCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AMarcus* FollowTarget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperZDAnimSequence* AttackAnimSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StoppingDistance = 70.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackCooldownInSeconds = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int HitPoints = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsAlive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsStunned = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanAttack = true;

	FTimerHandle StunTimer;
	FTimerHandle AttackCoolDownTimer;

	FZDOnAnimationOverrideEndSignature OnAttackOverrideEndDelegate;

	AEnemy();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
	void DetectorOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void DetectorOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool ShouldMoveToTarget() const;
	void UpdateDirection(float MoveDirection);

	void UpdateHitPoints(int NewHitPoints);
	void TakeDamage(int DamageAmount, float StunDuration, float PushBackForce);

	UFUNCTION(BlueprintCallable)
	void ApplyPushBack(float PushBackForce);

	void Stun(float StunDurationInSeconds);
	void OnStunTimerTimeout();

	void AttackPlayer();
	void OnAttackCooldownTimerTimeout();
	void OnAttackOverrideAnimEnd(bool Completed);
	
	UFUNCTION()
	void AttackCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void EnableAttackCollisionBox(bool Enabled);
};

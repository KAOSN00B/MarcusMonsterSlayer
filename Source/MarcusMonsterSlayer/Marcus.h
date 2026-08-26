// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperZDCharacter.h"

#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"

#include "GameFramework/SpringArmComponent.h"
#include "Engine/TimerHandle.h"
#include "PlayerHUD.h"
#include "MarcusMonsterHunterGameInstance.h"

#include "Sound/Soundbase.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"	
#include "InputActionValue.h"
#include "GameFramework/Controller.h"
#include "PaperZDAnimInstance.h"
#include "Components/BoxComponent.h"
#include "CollectableItem.h"
#include "Marcus.generated.h"


/**
 * 
 */
UCLASS()
class MARCUSMONSTERSLAYER_API AMarcus : public APaperZDCharacter
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBoxComponent* AttackCollisionBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* QuitAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	const UPaperZDAnimSequence* AttackAnimSequence;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperZDAnimSequence* DeathAnimSequence;

	FZDOnAnimationOverrideEndSignature OnAttackOverrideEndDelegate;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerHUD> PlayerHUDClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPlayerHUD* PlayerHUDWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UMarcusMonsterHunterGameInstance* MyGameInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* CollectItemSound;

	// if alive it can also move and attack, if dead it can't do anything
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsAlive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsActive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanAttack = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsStunned = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int AttackDamage = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int  HitPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MaxHitPoints = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackStunDuration = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SwordPushBackForce = 500.0f;

	FTimerHandle StunTimer;
	FTimerHandle GameOverTimer;

	AMarcus();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void Move(const FInputActionValue& Value);
	void JumpStarted(const FInputActionValue& Value);
	void JumpEnded(const FInputActionValue& Value);

	void Attack(const FInputActionValue& Value);
	void TakeDamage(int DamageAmount, float StunDuration);
	void Stun(float DurationInSeconds);
	void OnStunTimerTimeout();
	void UpdateHitPoints(int NewHitPoints, int NewMaxHitPoints);
	

	void UpdateDirection(float MoveDirection);
	
	void OnAttackOverrideAnimEnd(bool Completed);

	UFUNCTION()
	void AttackCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void EnableAttackCollisionBox(bool Enabled);

	void CollectItem(CollectableType ItemType, int Amount);

	void UnlockDoubleJump();

	void OnGameOverTimerTimeout();
	void DeactivatePlayer();
	
	void PauseGame();

	void UnPauseGame();

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Marcus.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "GameFramework/CharacterMovementComponent.h"

AMarcus::AMarcus()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	AttackCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollisionBox"));
	AttackCollisionBox->SetupAttachment(RootComponent);

}

void AMarcus::BeginPlay()
{
	Super::BeginPlay();

	HitPoints = MaxHitPoints;

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	OnAttackOverrideEndDelegate.BindUObject(this, &AMarcus::OnAttackOverrideAnimEnd);


	AttackCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AMarcus::AttackCollisionBoxBeginOverlap);
	EnableAttackCollisionBox(false);

	MyGameInstance = Cast<UMarcusMonsterHunterGameInstance>(GetGameInstance());

	if (MyGameInstance)
	{
		if (MyGameInstance->HasInitializedHP)
		{
			HitPoints = MyGameInstance->PlayerHP;
			MaxHitPoints = MyGameInstance->PlayerMaxHP;
		}
		else
		{
			MyGameInstance->SetPlayerHP(HitPoints, MaxHitPoints);
			MyGameInstance->HasInitializedHP = true;
		}

		if (MyGameInstance->IsDoubleJumpUnlocked)
		{
			MyGameInstance->IsDoubleJumpUnlocked = true;
			UnlockDoubleJump();
		}
	}

	//creating hud at start
	if (PlayerHUDClass)
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUD>(UGameplayStatics::GetPlayerController(GetWorld(), 0), PlayerHUDClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToPlayerScreen();
			PlayerHUDWidget->SetHP(HitPoints, MaxHitPoints);
			PlayerHUDWidget->SetMoney(MyGameInstance->CollectedMoneyCount); //currency and level are default vaules for test. will implement getters soon.
			PlayerHUDWidget->SetLevel(MyGameInstance->CurrentLevelIndex);
		}
	}
}

void AMarcus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMarcus::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMarcus::Move);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMarcus::JumpStarted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMarcus::JumpEnded);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Canceled, this, &AMarcus::JumpEnded);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AMarcus::Attack);
		EnhancedInputComponent->BindAction(QuitAction, ETriggerEvent::Started, this, &AMarcus::PauseGame);
		//EnhancedInputComponent->BindAction(QuitAction, ETriggerEvent::Completed, this, &AMarcus::UnPauseGame);
	}
}

void AMarcus::Move(const FInputActionValue& Value)
{
	//positive 1 is right, negative 1 is left (D and A keys)
	float MoveActionValue = Value.Get<float>();

	if (IsAlive && CanMove && !IsStunned)
	{
		FVector Direction = FVector(1.0f, 0.0f, 0.0f); // move in the X direction (right) not up or down Z or Y
		AddMovementInput(Direction, MoveActionValue); // built in function to add movement input, so we can use it directly
		UpdateDirection(MoveActionValue); // update the direction of the character based on the input value
	}
}

void AMarcus::UpdateDirection(float MoveDirection)
{
	FRotator CurrentRotation = Controller->GetControlRotation(); // get the current rotation of the controller
	if (MoveDirection < 0.0f) // if move direction is not 0 we are moving left (-1)
	{
		if (CurrentRotation.Yaw != 180.0f) // yaw is the Z axis
		{
			// set the rotation to 180 degrees on the Z axis (yaw) to face left)
			Controller->SetControlRotation(FRotator(CurrentRotation.Pitch, 180.0f, CurrentRotation.Roll));
		}
	}
	else if (MoveDirection > 0.0f)
	{
		if (CurrentRotation.Yaw != 0.0f)
		{
			// set the rotation to 0 degrees on the Z axis (yaw) to face right)
			Controller->SetControlRotation(FRotator(CurrentRotation.Pitch, 0.0f, CurrentRotation.Roll));
		}
	}
}

void AMarcus::JumpStarted(const FInputActionValue& Value)
{
	if (IsAlive && CanMove)
	{

		Jump(); // built in jump function, so we can use it directly
	}
}

void AMarcus::JumpEnded(const FInputActionValue& Value)
{

	StopJumping(); // built in stop jumping function, so we can use it directly
}

void AMarcus::Attack(const FInputActionValue& Value)
{

	if (IsAlive && CanAttack && !IsStunned)
	{
		CanAttack = false;
		CanMove = false;

		EnableAttackCollisionBox(true);

		GetAnimInstance()->PlayAnimationOverride(AttackAnimSequence, FName("DefaultSlot"), 1.0f,
			0.0f, OnAttackOverrideEndDelegate);
	}
}

void AMarcus::TakeDamage(int DamageAmount, float StunDuration)
{
	if (!IsAlive) return;
	if (!IsActive)return;

	UpdateHitPoints(HitPoints - DamageAmount, MaxHitPoints);

	if (HitPoints <= 0)
	{
		//Marcus is Dead		
		// place holder may make health bar instead. HPText->SetHiddenInGame(true); 
		UpdateHitPoints(0, MaxHitPoints);
		IsAlive = false;
		CanMove = false;
		CanAttack = false;

		float RestartDelay = 3.0f;

		GetWorldTimerManager().SetTimer(GameOverTimer, this, &AMarcus::OnGameOverTimerTimeout, 1.0f, false, RestartDelay);

		GetAnimInstance()->JumpToNode(FName("JumpDie"), FName("MarcusStateMachine"));
		EnableAttackCollisionBox(false);


	}
	else
	{
		GetAnimInstance()->JumpToNode(FName("JumpTakeHit"), FName("MarcusStateMachine"));
		Stun(StunDuration);
	}
}

void AMarcus::UpdateHitPoints(int NewHitPoints, int NewMaxHitPoints)
{
	HitPoints = NewHitPoints;
	MaxHitPoints = NewMaxHitPoints;

	if (HitPoints > MaxHitPoints)
	{
		HitPoints = MaxHitPoints;
	}

	MyGameInstance->SetPlayerHP(HitPoints, MaxHitPoints);

	PlayerHUDWidget->SetHP(HitPoints, MaxHitPoints);
}

void AMarcus::Stun(float StunDurationInSeconds)
{
	IsStunned = true;

	bool IsTimerAlreadyActive = GetWorldTimerManager().IsTimerActive(StunTimer); //if the timer is already active, clear it so we can reset it with the new stun duration

	if (IsTimerAlreadyActive)
	{
		GetWorldTimerManager().ClearTimer(StunTimer);
	}

	GetWorldTimerManager().SetTimer(StunTimer, this, &AMarcus::OnStunTimerTimeout, 1.0f, false, StunDurationInSeconds);
	GetAnimInstance()->StopAllAnimationOverrides(); //need to do this to stop the current animation override (hit) so we can play the stun animation override
}

void AMarcus::OnStunTimerTimeout()
{
	IsStunned = false;
}

void AMarcus::OnAttackOverrideAnimEnd(bool Completed)
{
	if(IsActive && IsAlive)
	{
		CanAttack = true;
		CanMove = true;
		EnableAttackCollisionBox(false);
	}
}

void AMarcus::AttackCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);

	if (Enemy)
	{
		Enemy->TakeDamage(AttackDamage, AttackStunDuration, SwordPushBackForce);
	}
}

void AMarcus::EnableAttackCollisionBox(bool Enabled)
{
	if (Enabled)
	{
		AttackCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AttackCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	else
	{
		AttackCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttackCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	}
}

void AMarcus::CollectItem(CollectableType ItemType, int Amount)
{
	UGameplayStatics::PlaySound2D(GetWorld(), CollectItemSound); //placeholder will change to each sound have its own sound

	switch (ItemType)
	{
	case CollectableType::HealthPotion:
	{
		UpdateHitPoints(HitPoints + Amount, MaxHitPoints);
	}break;

	case CollectableType::Money:
	{
		if (MyGameInstance)
		{
			MyGameInstance->AddMoney(Amount);
			
		}
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->SetMoney(MyGameInstance->CollectedMoneyCount);
		}
	}break;
	case CollectableType::DoubleJumpUpgrade:
	{
		if (!MyGameInstance->IsDoubleJumpUnlocked)
		{
			MyGameInstance->IsDoubleJumpUnlocked = true;
			UnlockDoubleJump();
		}
	}break;
	default:
	{

	}break;

	}
}

void AMarcus::UnlockDoubleJump()
{
	JumpMaxCount = 2;
}

void AMarcus::OnGameOverTimerTimeout()
{
	MyGameInstance->RestartGame();
}

void AMarcus::DeactivatePlayer()
{
	if (IsActive)
	{
		IsActive = false;
		CanAttack = false;
		CanMove = false;

		GetCharacterMovement()->StopMovementImmediately(); //all momentum gone from player
	}
}

void AMarcus::PauseGame()
{
	// Pause
	bool IsGamePaused = UGameplayStatics::IsGamePaused(GetWorld());
	UGameplayStatics::SetGamePaused(GetWorld(), !IsGamePaused);
}

void AMarcus::UnPauseGame()
{

	UGameplayStatics::SetGamePaused(GetWorld(), false);
}



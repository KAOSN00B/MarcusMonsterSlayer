// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerDetectorSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetectorSphere"));
	PlayerDetectorSphere->SetupAttachment(RootComponent);

	AttackCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollisionBox"));
	AttackCollisionBox->SetupAttachment(RootComponent);

	HPText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HPText"));
	HPText->SetupAttachment(RootComponent);

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyHealthBar"));
	HealthBarWidget->SetupAttachment(RootComponent);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	HitPoints = MaxHitPoints;

	HealthBarWidget->SetVisibility(false);
	HPText->SetVisibility(false);

	PlayerDetectorSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::DetectorOverlapBegin);
	PlayerDetectorSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::DetectorOverlapEnd);

	UpdateHitPoints(HitPoints);

	OnAttackOverrideEndDelegate.BindUObject(this, &AEnemy::OnAttackOverrideAnimEnd);
	OnDeathOverrideEndDelegate.BindUObject(this, &AEnemy::OnDeathOverrideAnimEnd);

	AttackCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AttackCollisionBoxBeginOverlap);
	EnableAttackCollisionBox(false);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsAlive && FollowTarget && !IsStunned)
	{
		// get the direction to move in based on the target's location                    Ternary operator (Like an if Statement)
		float MoveDirection = (FollowTarget->GetActorLocation().X - GetActorLocation().X) > 0.0f ? 1.0f : -1.0f;
		UpdateDirection(MoveDirection);
		if (ShouldMoveToTarget())
		{
			if (CanMove)
			{
				FVector WorldDirection = FVector(1.0f, 0.0f, 0.0f); // move in the X direction (right) not up or down Z or Y
				AddMovementInput(WorldDirection, MoveDirection);

			}
		}
		else
		{
			if (FollowTarget->IsAlive && CanAttack)
			{
				AttackPlayer();
			}
		}
	}
}

void AEnemy::UpdateDirection(float MoveDirection)
{
	FRotator CurrentRotation = GetActorRotation(); // get the current rotation of the controller
	if (MoveDirection < 0.0f) // if move direction is not 0 we are moving left (-1)
	{
		// set the rotation to 180 degrees on the Z axis (yaw) to face left)
			SetActorRotation(FRotator(CurrentRotation.Pitch, 180.0f, CurrentRotation.Roll));

	}
	else if (MoveDirection > 0.0f)
	{
		// set the rotation to 0 degrees on the Z axis (yaw) to face right)
			SetActorRotation(FRotator(CurrentRotation.Pitch, 0.0f, CurrentRotation.Roll));

	}
}


bool AEnemy::ShouldMoveToTarget() const
{
	bool Result = false;

	if (FollowTarget)
	{
		float DistanceToTarget = abs(FollowTarget->GetActorLocation().X - GetActorLocation().X);
		Result = DistanceToTarget > StoppingDistance;
	}

	return Result;
}



void AEnemy::DetectorOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMarcus* Player = Cast<AMarcus>(OtherActor);
	if (Player)
	{
		FollowTarget = Player;
	}

}

void AEnemy::DetectorOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMarcus* Player = Cast<AMarcus>(OtherActor);
	if (Player)
	{
		FollowTarget = NULL;
	}
}

void AEnemy::UpdateHitPoints(int NewHitPoints)
{
	HitPoints = NewHitPoints;

	FString HitPointString = FString::Printf(TEXT("HP: %d/%d"), HitPoints, MaxHitPoints);
	HPText->SetText(FText::FromString(HitPointString));

	UEnemyHealthBar* EnemyHealthBar = Cast<UEnemyHealthBar>(HealthBarWidget->GetUserWidgetObject());

	if (EnemyHealthBar)
	{
		EnemyHealthBar->SetHealth(HitPoints, MaxHitPoints);
	}
}

void AEnemy::TakeDamage(int DamageAmount, float StunDuration, float PushBackForce)
{
	if (!IsAlive) return;

	UpdateHitPoints(HitPoints - DamageAmount);

	HealthBarWidget->SetVisibility(true);
	HPText->SetVisibility(true);

		if (HitPoints <= 0)
		{
			//Enemy is Dead
			UpdateHitPoints(0);
			HPText->SetHiddenInGame(true); //enemy is dead , hide the HP text (will change to hp bar soon)
			IsAlive = false;
			CanMove = false;
			CanAttack = false;


			HealthBarWidget->SetVisibility(false);
			HPText->SetVisibility(false);

			GetAnimInstance()->JumpToNode(FName("JumpDie"), FName("OgreStateMachine"));
			EnableAttackCollisionBox(false);

			GetAnimInstance()->PlayAnimationOverride(DeathAnimSequence, FName("DefaultSlot"),
				1.0f, 0.0f, OnDeathOverrideEndDelegate);
		}
		else
		{
			GetAnimInstance()->JumpToNode(FName("JumpTakeHit"), FName("OgreStateMachine"));
			ApplyPushBack(PushBackForce);
			Stun(StunDuration);
		}
}

void AEnemy::ApplyPushBack(float PushBackForce)
{
	if (IsAlive && !IsStunned && FollowTarget)
	{
		FVector PushBackDirection = (GetActorLocation() - FollowTarget->GetActorLocation()).GetSafeNormal();

		LaunchCharacter(PushBackDirection * PushBackForce, true, true);
	}
}

void AEnemy::Stun(float StunDurationInSeconds)
{
	IsStunned = true;

	bool IsTimerAlreadyActive = GetWorldTimerManager().IsTimerActive(StunTimer); //if the timer is already active, clear it so we can reset it with the new stun duration

	if (IsTimerAlreadyActive)
	{
		GetWorldTimerManager().ClearTimer(StunTimer);
	}

	GetWorldTimerManager().SetTimer(StunTimer, this, &AEnemy::OnStunTimerTimeout, 1.0f, false, StunDurationInSeconds);
	GetAnimInstance()->StopAllAnimationOverrides(); //need to do this to stop the current animation override (hit) so we can play the stun animation override
}

void AEnemy::OnStunTimerTimeout()
{
	IsStunned = false;

}

void AEnemy::AttackPlayer()
{
	if (CanAttack && IsAlive && !IsStunned)
	{
		CanAttack = false;
		CanMove = false;

		GetAnimInstance()->PlayAnimationOverride(AttackAnimSequence, FName("DefaultSlot"), 1.0f,
			0.0f, OnAttackOverrideEndDelegate);
		GetWorldTimerManager().SetTimer(AttackCoolDownTimer, this,
			&AEnemy::OnAttackCooldownTimerTimeout, 1.0f, false, AttackCooldownInSeconds);

	}
}

void AEnemy::OnAttackOverrideAnimEnd(bool Completed)
{
	if(IsAlive)
	{
		CanMove = true;
	}
}

void AEnemy::OnAttackCooldownTimerTimeout()
{
	if (IsAlive)
	{
		CanAttack = true;
	}
}

void AEnemy::AttackCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMarcus* Player = Cast<AMarcus>(OtherActor);

	if (Player)
	{
		Player->TakeDamage(AttackPower, AttackStunDuration);
	}

}

void AEnemy::EnableAttackCollisionBox(bool Enabled)
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

void AEnemy::OnDeathOverrideAnimEnd(bool Completed)
{
	Destroy();
}

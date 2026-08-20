// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerDetectorSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PlayerDetectorSphere"));
	PlayerDetectorSphere->SetupAttachment(RootComponent);

	HPText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HPText"));
	HPText->SetupAttachment(RootComponent);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	PlayerDetectorSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::DetectorOverlapBegin);
	PlayerDetectorSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::DetectorOverlapEnd);

	UpdateHitPoints(HitPoints);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsAlive && FollowTarget)
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
			//AttackPlayer();
		}
	}
}

void AEnemy::UpdateDirection(float MoveDirection)
{
	FRotator CurrentRotation = GetActorRotation(); // get the current rotation of the controller
	if (MoveDirection < 0.0f) // if move direction is not 0 we are moving left (-1)
	{
			// set the rotation to 180 degrees on the Z axis (yaw) to face left)
			SetActorRotation(FRotator(CurrentRotation.Pitch, 0.0f, CurrentRotation.Roll));

	}
	else if (MoveDirection > 0.0f)
	{
			// set the rotation to 0 degrees on the Z axis (yaw) to face right)
			SetActorRotation(FRotator(CurrentRotation.Pitch, 180.0f, CurrentRotation.Roll));

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

	FString HitPointString = FString::Printf(TEXT("HP: %d"), HitPoints);
	HPText->SetText(FText::FromString(HitPointString));
}

void AEnemy::TakeDamage(int DamageAmount, float StunDuration)
{
	if (!IsAlive) return;

	UpdateHitPoints(HitPoints - DamageAmount);

	if(HitPoints <= 0)
	{
		//Enemy is Dead
		UpdateHitPoints(0);
		HPText->SetHiddenInGame(true); //enemy is dead , hide the HP text (will change to hp bar soon)
		IsAlive = false;
		CanMove = false;

		//Play Death Animation override eventually
	}
	else
	{
		//Enemy is still alive take hit animation override eventually

	}
}


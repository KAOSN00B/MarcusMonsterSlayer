// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTriggerBox.h"
#include "Marcus.h"
#include "GameFramework/CharacterMovementComponent.h"

AMyTriggerBox::AMyTriggerBox() {}

void AMyTriggerBox::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AMyTriggerBox::OnOverlapBegin);
	
	if (DisappearsOnTimer)
	{
		if (StartDelay > 0.0f)
		{
			GetWorldTimerManager().SetTimer(CycleTimer, this,
				&AMyTriggerBox::ActivateHazard, StartDelay, false);
		}
		else
		{
			ActivateHazard();
		}
	}

}

void AMyTriggerBox::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	AMarcus* PlayerCharacter = Cast<AMarcus>(OtherActor);
	if (!PlayerCharacter) return;

	PlayerCharacter->TakeDamage(DamageToPlayer, StunPlayerAmount);
	
	if (TeleportPlayerOnHit)
	{
		PlayerCharacter->GetCharacterMovement()->DisableMovement();
		PendingTeleportPlayer = PlayerCharacter;

		GetWorldTimerManager().SetTimer(TeleportDelayTimer, this,
			&AMyTriggerBox::DoDelayedTeleport, TeleportDelay, false);
	}

}

void AMyTriggerBox::TeleportPlayerToLastLocation(AMarcus* Player)
{
	if (!Player) return;

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow,
		FString::Printf(TEXT("Teleport to %s"), *Player->PlayerLastGroundedLocation.ToString()));


	Player->SetActorLocation(Player->PlayerLastGroundedLocation, false, nullptr,
		ETeleportType::TeleportPhysics);

	Player->GetCharacterMovement()->StopMovementImmediately();
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AMyTriggerBox::ActivateHazard()
{
	SetActorHiddenInGame(false); // appears
	SetActorEnableCollision(true);

	GetWorldTimerManager().SetTimer(CycleTimer, this,
		&AMyTriggerBox::DeactiveHazard, ActiveDuration, false);

}

void AMyTriggerBox::DeactiveHazard()
{
	SetActorHiddenInGame(true); //done opposite as active
	SetActorEnableCollision(false);

	GetWorldTimerManager().SetTimer(CycleTimer, this,
		&AMyTriggerBox::ActivateHazard, InactiveDuration, false);
}

void AMyTriggerBox::DoDelayedTeleport()
{
	TeleportPlayerToLastLocation(PendingTeleportPlayer);
	PendingTeleportPlayer = nullptr;
}

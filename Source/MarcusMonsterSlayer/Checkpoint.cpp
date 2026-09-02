// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "Marcus.h"
#include "MarcusMonsterHunterGameInstance.h"
#include "Components/BoxComponent.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/GameplayStatics.h"

ACheckpoint::ACheckpoint()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->SetGenerateOverlapEvents(true);

	Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	Flipbook->SetupAttachment(RootComponent);
}

void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::OnOverlapBegin);

	// after a checkpoint respawn the level reloads - show this one already lit if it's the active one
	bool bIsActiveCheckpoint = false;
	if (UMarcusMonsterHunterGameInstance* GI =
		Cast<UMarcusMonsterHunterGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		const FVector Here = GetActorLocation();
		bIsActiveCheckpoint = GI->HasCheckpoint
			&& FMath::IsNearlyEqual(GI->CheckpointLocation.X, Here.X, 1.0f)
			&& FMath::IsNearlyEqual(GI->CheckpointLocation.Z, Here.Z, 1.0f);
	}

	if (bIsActiveCheckpoint)
	{
		bActivated = true;
		if (ActiveAnim)
		{
			Flipbook->SetFlipbook(ActiveAnim);
		}
	}
	else if (InactiveAnim)
	{
		Flipbook->SetFlipbook(InactiveAnim);
	}
}

void ACheckpoint::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bActivated) return;

	AMarcus* Player = Cast<AMarcus>(OtherActor);
	if (!Player) return;

	bActivated = true;

	if (UMarcusMonsterHunterGameInstance* GI =
		Cast<UMarcusMonsterHunterGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		// checkpoint's X/Z, but keep Marcus on his own gameplay/depth plane
		FVector SpawnPoint = GetActorLocation();
		SpawnPoint.Y = Player->GetActorLocation().Y;

		GI->SetCheckpoint(FName(*UGameplayStatics::GetCurrentLevelName(this, true)), SpawnPoint);
	}

	if (ActiveAnim)
	{
		Flipbook->SetFlipbook(ActiveAnim);
	}

	if (ActivateSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ActivateSound, GetActorLocation());
	}
}

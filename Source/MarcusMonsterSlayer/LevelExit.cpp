// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelExit.h"
#include "Marcus.h"
#include "Kismet/GameplayStatics.h"

#include "MarcusMonsterHunterGameInstance.h"


ALevelExit::ALevelExit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CapsuleComponent"));
	RootComponent = BoxComp;

	DoorFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("ItemFlipbook"));
	DoorFlipbook->SetupAttachment(RootComponent);

	DoorFlipbook->SetPlayRate(0.0f);
	DoorFlipbook->SetLooping(false);
}

// Called when the game starts or when spawned
void ALevelExit::BeginPlay()
{
	Super::BeginPlay();
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &ALevelExit::OverlapBegin);

	DoorFlipbook->SetPlaybackPosition(0.0f, false); //resets anim to closed because unreal is wild at times.
}

// Called every frame
void ALevelExit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelExit::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMarcus* Player = Cast<AMarcus>(OtherActor);

	if (Player && Player->IsAlive)
	{
		if (IsActive)
		{
			Player->DeactivatePlayer();

			IsActive = false;
			DoorFlipbook->SetPlayRate(1.0f);
			DoorFlipbook->PlayFromStart();

			UGameplayStatics::PlaySound2D(GetWorld(), PlayEnterSound);

			GetWorldTimerManager().SetTimer(WaitTimer, this, &ALevelExit::OnWaitTimerTimeout,
				1.0f, false, WaitTimeInSeconds);
		}
	}
}

void ALevelExit::OnWaitTimerTimeout()
{
	UMarcusMonsterHunterGameInstance* MyGameInstance = 
		Cast<UMarcusMonsterHunterGameInstance>(GetGameInstance());

	if (MyGameInstance)
	{
		MyGameInstance->ChangeLevel(NextLevel);
	}

}

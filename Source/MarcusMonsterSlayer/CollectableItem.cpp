// Fill out your copyright notice in the Description page of Project Settings.


#include "CollectableItem.h"
#include "Marcus.h"

// Sets default values
ACollectableItem::ACollectableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComp;

	ItemFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("ItemFlipbook"));
	ItemFlipbook->SetupAttachment(RootComponent);
}


// Called when the game starts or when spawned
void ACollectableItem::BeginPlay()
{
	Super::BeginPlay();

	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ACollectableItem::OverlapBegin);
}

// Called every frame
void ACollectableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ACollectableItem::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AMarcus* Player = Cast<AMarcus>(OtherActor);

	if (Player && Player->IsAlive)
	{
		Player->CollectItem(Type);
		Destroy();
	}
}

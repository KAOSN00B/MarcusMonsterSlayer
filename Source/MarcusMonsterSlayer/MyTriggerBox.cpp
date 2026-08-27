// Fill out your copyright notice in the Description page of Project Settings.


#include "MyTriggerBox.h"
#include "Marcus.h"

AMyTriggerBox::AMyTriggerBox() {}

void AMyTriggerBox::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &AMyTriggerBox::OnOverlapBegin);
}

void AMyTriggerBox::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	AMarcus* PlayerCharacter = Cast<AMarcus>(OtherActor);
	if (!PlayerCharacter) return;

	PlayerCharacter->DeactivatePlayer();
	PlayerCharacter->PlayerDeath();

}
// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingEnemy.h"
#include "Marcus.h"
#include "Components/BoxComponent.h"
#include "PaperFlipbookComponent.h"
#include "Kismet/GameplayStatics.h"

AFlyingEnemy::AFlyingEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	ContactBox = CreateDefaultSubobject<UBoxComponent>(TEXT("ContactBox"));
	RootComponent = ContactBox;
	ContactBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ContactBox->SetCollisionObjectType(ECC_Pawn);            // so Marcus's sword (overlaps Pawn) can hit it
	ContactBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	ContactBox->SetGenerateOverlapEvents(true);

	Flipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("Flipbook"));
	Flipbook->SetupAttachment(RootComponent);
}

void AFlyingEnemy::BeginPlay()
{
	Super::BeginPlay();

	HitPoints = MaxHitPoints;

	PatrolStart = GetActorLocation();
	PatrolEnd = PatrolStart + PatrolOffset;

	ContactBox->OnComponentBeginOverlap.AddDynamic(this, &AFlyingEnemy::OnContactBegin);
	Flipbook->OnFinishedPlaying.AddDynamic(this, &AFlyingEnemy::OnDeathAnimFinished);
}

void AFlyingEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDead) return;

	PatrolTime += DeltaTime * PatrolSpeed;

	// smooth 0 -> 1 -> 0 ping-pong (ease in/out at each end)
	const float Alpha = 0.5f - 0.5f * FMath::Cos(PatrolTime * 2.0f * PI);

	const FVector PreviousLocation = GetActorLocation();
	const FVector NewLocation = FMath::Lerp(PatrolStart, PatrolEnd, Alpha);
	SetActorLocation(NewLocation);

	if (FaceTravelDirection)
	{
		const float DeltaX = NewLocation.X - PreviousLocation.X;
		if (FMath::Abs(DeltaX) > KINDA_SMALL_NUMBER)
		{
			FVector Scale = Flipbook->GetRelativeScale3D();
			Scale.X = DeltaX > 0.0f ? FMath::Abs(Scale.X) : -FMath::Abs(Scale.X);
			Flipbook->SetRelativeScale3D(Scale);
		}
	}
}

void AFlyingEnemy::TakeHit(int Damage)
{
	if (bDead) return;

	HitPoints -= Damage;
	if (HitPoints > 0) return;

	bDead = true;

	// stop being an obstacle - no more patrol, no more contact damage
	ContactBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if (DeathAnim)
	{
		Flipbook->SetLooping(false);
		Flipbook->SetFlipbook(DeathAnim);
		// keep the current facing sign, apply the death-anim size multiplier
		Flipbook->SetRelativeScale3D(Flipbook->GetRelativeScale3D() * DeathAnimScale);
		Flipbook->PlayFromStart();
	}
	else
	{
		Destroy();
	}
}

void AFlyingEnemy::OnContactBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bDead) return;

	if (AMarcus* Player = Cast<AMarcus>(OtherActor))
	{
		Player->TakeDamage(ContactDamage, ContactStunDuration);
	}
}

void AFlyingEnemy::OnDeathAnimFinished()
{
	// only the non-looping death flipbook ever fires this
	if (bDead)
	{
		Destroy();
	}
}

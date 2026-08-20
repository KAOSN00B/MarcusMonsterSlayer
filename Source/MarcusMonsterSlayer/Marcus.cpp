// Fill out your copyright notice in the Description page of Project Settings.


#include "Marcus.h"
#include "Enemy.h"

AMarcus::AMarcus()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	AttackCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackCollisionBox"));
	AttackCollisionBox->SetupAttachment(RootComponent);
}

void AMarcus::BeginPlay()
{
	Super::BeginPlay();

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
	}
}

void AMarcus::Move(const FInputActionValue& Value)
{
	//positive 1 is right, negative 1 is left (D and A keys)
	float MoveActionValue = Value.Get<float>();

	if (IsAlive && CanMove)
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

	if (IsAlive && CanAttack)
	{
		CanAttack = false;
		CanMove = false;

		EnableAttackCollisionBox(true);

		GetAnimInstance()->PlayAnimationOverride(AttackAnimSequence, FName("DefaultSlot"), 1.0f,
			0.0f, OnAttackOverrideEndDelegate);
	}
}

void AMarcus::OnAttackOverrideAnimEnd(bool Completed)
{
	CanAttack = true;
	CanMove = true;
	EnableAttackCollisionBox(false);
}

void AMarcus::AttackCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnemy* Enemy = Cast<AEnemy>(OtherActor);

	if(Enemy)
	{
		Enemy->TakeDamage(AttackDamage, AttackStunDuration);
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




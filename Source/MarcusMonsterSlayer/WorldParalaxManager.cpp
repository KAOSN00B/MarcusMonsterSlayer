// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldParalaxManager.h"

#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraComponent.h"
#include "PaperSpriteComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"

AWorldParalaxManager::AWorldParalaxManager()
{
	PrimaryActorTick.bCanEverTick = true;

	// Tick after the camera (and its lag) has finished updating this frame
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;
}

void AWorldParalaxManager::BeginPlay()
{
	Super::BeginPlay();
	InitialiseFromCurrentState();
}

void AWorldParalaxManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsInitialised)
	{
		InitialiseFromCurrentState();
		if (!IsInitialised)
		{
			return;
		}
	}

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!CameraManager)
	{
		return;
	}

	const FVector CameraOffset = CameraManager->GetCameraLocation() - InitialCameraLocation;

	for (const FParallaxLayer& Layer : ParallaxLayers)
	{
		if (!Layer.LayerActor)
		{
			continue;
		}

		FVector NewLocation = Layer.InitialLocation;
		NewLocation.X += CameraOffset.X * (1.0f - Layer.ParallaxFactor);
		NewLocation.Z += CameraOffset.Z * (1.0f - Layer.ParallaxFactor);

		Layer.LayerActor->SetActorLocation(NewLocation);
	}
}

void AWorldParalaxManager::InitialiseFromCurrentState()
{
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!CameraManager)
	{
		return;
	}

	InitialCameraLocation = CameraManager->GetCameraLocation();

	for (FParallaxLayer& Layer : ParallaxLayers)
	{
		if (!Layer.LayerActor)
		{
			continue;
		}

		// Parallax moves the actor every frame, so it can't be Static
		if (USceneComponent* Root = Layer.LayerActor->GetRootComponent())
		{
			Root->SetMobility(EComponentMobility::Movable);
		}

		if (Layer.bFitToView)
		{
			FitLayerToView(Layer, InitialCameraLocation);
		}

		Layer.InitialLocation = Layer.LayerActor->GetActorLocation();
	}

	IsInitialised = true;
}

void AWorldParalaxManager::FitLayerToView(FParallaxLayer& Layer, const FVector& CameraLocation)
{
	// Snap the layer to the camera, keeping its authored depth on Y
	const FVector Current = Layer.LayerActor->GetActorLocation();
	Layer.LayerActor->SetActorLocation(FVector(CameraLocation.X, Current.Y, CameraLocation.Z));

	UPaperSpriteComponent* SpriteComp = Layer.LayerActor->FindComponentByClass<UPaperSpriteComponent>();
	if (!SpriteComp)
	{
		return;
	}

	// Orthographic view size, in world units
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	AActor* ViewTarget = CameraManager ? CameraManager->GetViewTarget() : nullptr;
	UCameraComponent* Camera = ViewTarget ? ViewTarget->FindComponentByClass<UCameraComponent>() : nullptr;
	if (!Camera)
	{
		return;
	}

	const float ViewWidth = Camera->OrthoWidth;

	float AspectRatio = 1.7777f;
	if (GEngine && GEngine->GameViewport && GEngine->GameViewport->Viewport)
	{
		const FIntPoint ViewportSize = GEngine->GameViewport->Viewport->GetSizeXY();
		if (ViewportSize.Y > 0)
		{
			AspectRatio = static_cast<float>(ViewportSize.X) / static_cast<float>(ViewportSize.Y);
		}
	}
	const float ViewHeight = ViewWidth / AspectRatio;

	// Native (unscaled) sprite size in world units - sprite lives in the XZ plane
	const FBoxSphereBounds LocalBounds = SpriteComp->CalcBounds(FTransform::Identity);
	const float NativeWidth = LocalBounds.BoxExtent.X * 2.0f;
	const float NativeHeight = LocalBounds.BoxExtent.Z * 2.0f;
	if (NativeWidth <= KINDA_SMALL_NUMBER || NativeHeight <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	// Uniform scale that covers the view without stretching, times the overhang margin
	const float CoverScale = FMath::Max(ViewWidth / NativeWidth, ViewHeight / NativeHeight);
	const float FinalScale = CoverScale * FMath::Max(Layer.FitMargin, 1.0f);

	SpriteComp->SetWorldScale3D(FVector(FinalScale, 1.0f, FinalScale));
}

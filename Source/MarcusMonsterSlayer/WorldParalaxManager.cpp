// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldParalaxManager.h"

#include "PaperSpriteActor.h"
#include "PaperSpriteComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"

namespace
{
	const FName CameraPosXParam(TEXT("CameraPosX"));
	const FName ParalaxMultiplierParam(TEXT("Paralax Multiplyer"));
}

AWorldParalaxManager::AWorldParalaxManager()
{
	PrimaryActorTick.bCanEverTick = true;

	// Tick after the camera (and its lag) has finished updating this frame, so the
	// material always reflects the camera position actually used for rendering.
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;
}

void AWorldParalaxManager::BeginPlay()
{
	Super::BeginPlay();

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	AActor* ViewTarget = CameraManager ? CameraManager->GetViewTarget() : nullptr;
	UCameraComponent* Camera = ViewTarget ? ViewTarget->FindComponentByClass<UCameraComponent>() : nullptr;

	for (FParallaxLayer& Layer : ParallaxLayers)
	{
		if (!Layer.LayerActor)
		{
			continue;
		}

		// A Static component can't be attached under a Movable parent, so it would
		// silently fail to follow - force it Movable before attaching.
		if (USceneComponent* Root = Layer.LayerActor->GetRootComponent())
		{
			Root->SetMobility(EComponentMobility::Movable);
		}

		if (Layer.bFitToView)
		{
			FitLayerToView(Layer, Camera);
		}

		// Attach to the camera (not the player pawn) so this layer always stays in view
		// without inheriting any left/right flip the player does when turning around.
		if (Camera)
		{
			// Depth relative to the camera, not an absolute world Y - otherwise this
			// drifts by the camera's own world position and can Z-fight with other layers.
			const float RelativeDepth = Layer.LayerActor->GetActorLocation().Y - Camera->GetComponentLocation().Y;
			Layer.LayerActor->AttachToComponent(Camera, FAttachmentTransformRules::KeepWorldTransform);
			Layer.LayerActor->SetActorRelativeLocation(FVector(Layer.Offset.X, RelativeDepth, Layer.Offset.Y));
		}

		UPaperSpriteComponent* Sprite = Layer.LayerActor->GetRenderComponent();
		UMaterialInterface* SourceMaterial = Sprite ? Sprite->GetMaterial(0) : nullptr;
		if (!SourceMaterial)
		{
			continue;
		}

		Layer.DynamicMaterial = UMaterialInstanceDynamic::Create(SourceMaterial, this);
		Sprite->SetMaterial(0, Layer.DynamicMaterial);
		Layer.DynamicMaterial->SetScalarParameterValue(ParalaxMultiplierParam, Layer.Speed);
	}
}

void AWorldParalaxManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!CameraManager)
	{
		return;
	}

	const float CameraPosX = CameraManager->GetCameraLocation().X;

	for (const FParallaxLayer& Layer : ParallaxLayers)
	{
		if (Layer.DynamicMaterial)
		{
			Layer.DynamicMaterial->SetScalarParameterValue(CameraPosXParam, CameraPosX);
		}
	}
}

void AWorldParalaxManager::FitLayerToView(FParallaxLayer& Layer, UCameraComponent* Camera)
{
	if (!Camera)
	{
		return;
	}

	UPaperSpriteComponent* SpriteComp = Layer.LayerActor->FindComponentByClass<UPaperSpriteComponent>();
	if (!SpriteComp)
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

	// Native (unscaled) sprite size in world units - sprite lives in the XZ plane.
	const FBoxSphereBounds LocalBounds = SpriteComp->CalcBounds(FTransform::Identity);
	const float NativeWidth = LocalBounds.BoxExtent.X * 2.0f;
	const float NativeHeight = LocalBounds.BoxExtent.Z * 2.0f;
	if (NativeWidth <= KINDA_SMALL_NUMBER || NativeHeight <= KINDA_SMALL_NUMBER)
	{
		return;
	}

	// Uniform scale that covers the view without stretching, times the overhang margin.
	const float CoverScale = FMath::Max(ViewWidth / NativeWidth, ViewHeight / NativeHeight);
	const float FinalScale = CoverScale * FMath::Max(Layer.FitMargin, 1.0f);

	SpriteComp->SetWorldScale3D(FVector(FinalScale, 1.0f, FinalScale));
}

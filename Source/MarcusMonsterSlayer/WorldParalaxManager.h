// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldParalaxManager.generated.h"

USTRUCT(BlueprintType)
struct FParallaxLayer
{
	GENERATED_BODY()

	// A background actor already placed in the level (e.g. a PaperSpriteActor)
	UPROPERTY(EditAnywhere, Category = "Parallax")
	AActor* LayerActor = nullptr;

	// 0 = locked to the camera (infinitely far).  1 = locked to the world (foreground).
	UPROPERTY(EditAnywhere, Category = "Parallax", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float ParallaxFactor = 0.5f;

	// At startup, scale this layer to cover the camera view and snap it to the camera.
	// Use it for the far backdrop layer.
	UPROPERTY(EditAnywhere, Category = "Parallax")
	bool bFitToView = false;

	// 1.0 = cover the view exactly.  >1 = overhang, so a scrolling layer never shows an edge.
	UPROPERTY(EditAnywhere, Category = "Parallax", meta = (ClampMin = "1.0", EditCondition = "bFitToView"))
	float FitMargin = 1.0f;

	// Runtime scratch - where this actor started
	FVector InitialLocation = FVector::ZeroVector;
};

UCLASS()
class MARCUSMONSTERSLAYER_API AWorldParalaxManager : public AActor
{
	GENERATED_BODY()

public:
	AWorldParalaxManager();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Parallax")
	TArray<FParallaxLayer> ParallaxLayers;

	// Camera position when we started - our reference point
	FVector InitialCameraLocation = FVector::ZeroVector;

	bool IsInitialised = false;

	void InitialiseFromCurrentState();
	void FitLayerToView(FParallaxLayer& Layer, const FVector& CameraLocation);
};

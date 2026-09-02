// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldParalaxManager.generated.h"

class APaperSpriteActor;
class UMaterialInstanceDynamic;
class UCameraComponent;

USTRUCT(BlueprintType)
struct FParallaxLayer
{
	GENERATED_BODY()

	// A Paper Sprite Actor already placed in the level. At BeginPlay it gets attached to the
	// player and snapped to Offset below, so it always stays in view.
	// Give its sprite a material based on M_ParalaxSprite (exposing "CameraPosX" and
	// "Paralax Multiplyer" scalar parameters).
	UPROPERTY(EditAnywhere, Category = "Parallax")
	APaperSpriteActor* LayerActor = nullptr;

	// Position relative to the camera once attached. X = left/right, Y = up/down.
	// Push Y up for a sky layer, or down/back on X-Y for something further away.
	UPROPERTY(EditAnywhere, Category = "Parallax")
	FVector2D Offset = FVector2D::ZeroVector;

	// 0 = doesn't scroll at all (attached solidly to the camera). 1 = scrolls at full camera speed.
	UPROPERTY(EditAnywhere, Category = "Parallax", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Speed = 0.5f;

	// Scale this layer at BeginPlay to cover the camera's view, so one sprite is enough
	// without you having to hand-size it.
	UPROPERTY(EditAnywhere, Category = "Parallax")
	bool bFitToView = true;

	// 1.0 = cover the view exactly. >1 = overhang, so it never shows an edge.
	UPROPERTY(EditAnywhere, Category = "Parallax", meta = (ClampMin = "1.0", EditCondition = "bFitToView"))
	float FitMargin = 1.2f;

	UPROPERTY(Transient)
	UMaterialInstanceDynamic* DynamicMaterial = nullptr;
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

	void FitLayerToView(FParallaxLayer& Layer, UCameraComponent* Camera);
};

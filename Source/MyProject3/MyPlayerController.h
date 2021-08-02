// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Misc/Optional.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MYPROJECT3_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

	AActor* Target;
	AActor* Dst;
	bool Stopping;
	bool NoAccel;
	bool UpdateDiff;
	bool Direction;
	bool DirectionChanged;
	int Frames;
	FVector Diff;
	FVector Neutral;
public:
	AMyPlayerController();
	~AMyPlayerController();

	virtual void Tick(float delta) override;
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;

	void Stop(bool stop)
	{
		Stopping = stop;
		if (stop) 
			UpdateDiff = false;
		else
			NoAccel = false;
	};

	void ChangeDirection(bool dir)
	{
		if (dir != Direction)  {
			Direction = dir;
			DirectionChanged = true;
			Stop(false);
		}
	}

};

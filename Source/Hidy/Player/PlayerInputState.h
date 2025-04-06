// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerInputState.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FPlayerInputState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToSprint = false;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToWalk = false;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToStrafe = false;

	UPROPERTY(BlueprintReadOnly)
	bool bWantsToAim = false;
};

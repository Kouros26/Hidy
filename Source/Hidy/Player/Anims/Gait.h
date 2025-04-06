// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gait.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EGait : uint8
{
	WALK UMETA(DisplayName="Walk"),
	RUN UMETA(DisplayName = "Run"),
	SPRINT UMETA(DisplayName = "Sprint")
};

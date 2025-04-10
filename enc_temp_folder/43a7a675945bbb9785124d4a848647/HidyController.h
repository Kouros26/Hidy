// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
#include "GameFramework/PlayerController.h"
#include "HidyController.generated.h"

/**
 * 
 */

UCLASS()
class HIDY_API AHidyController : public APlayerController
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IA_Look;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IA_LookGamepad;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IA_Walk;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IA_Sprint;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IA_Crouch;

	//-------------------------------------------

	void HidyController();

	//virtual void SetupInputComponent() override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
#include "PlayerInputState.h"
#include "GameFramework/PlayerController.h"
#include "HidyController.generated.h"

/**
 * 
 */

UCLASS()
class HIDY_API AHidyController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IA_Look;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IA_Walk;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IA_Sprint;

	UPROPERTY(EditDefaultsOnly, Category = Input)
	UInputAction* IA_Crouch;

	//-------------------------------------------

protected:

	UPROPERTY(Replicated)
	FPlayerInputState InputState;

public:

	void HidyController();

	FPlayerInputState GetInputState() const;
	void SetInputState(FPlayerInputState Other);

protected:

	void BeginPlay() override;

	virtual void SetupInputComponent() override;

private:

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void WalkToggle(const FInputActionValue& Value);

	void Sprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);

	void Crouch(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void RPC_Server_UpdateInputState(const FPlayerInputState State);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};

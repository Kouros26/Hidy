// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "PlayerInputState.h"
#include "Anims/Gait.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

class USpotLightComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class HIDY_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	class AHidyController* HidyController = nullptr;
	UCharacterMovementComponent* Movement = nullptr;

	class UPreCMCTick* PreTick = nullptr;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Light)
	USpotLightComponent* Spotlight;

	UPROPERTY(Replicated)
	FPlayerInputState InputState;

	FRichCurve StrafeSpeedMapCurve;

	UPROPERTY(BlueprintReadOnly, Category = Anim)
	EGait Gait = EGait::RUN;

	FVector3f WalkSpeeds = { 200, 180, 150 };
	FVector3f RunSpeeds = { 500, 350, 300 };
	FVector3f SprintSpeeds = { 700, 700, 700 };
	FVector3f CrouchSpeeds = { 225, 200, 180 };

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	EGait GetDesiredGait() const;
	float CalculateMaxAcceleration() const;
	float CalculateMaxSpeed() const;
	float CalculateMaxCrouchSpeed() const;
	float CalculateBreakingDeceleration() const;
	float CalculateGroundFriction() const;

	FPlayerInputState GetInputState() const;
	void SetInputState(FPlayerInputState Other);

	constexpr EGait GetGait() const { return Gait; }
	constexpr void SetGait(const EGait newGait) { Gait = newGait; }

	constexpr AHidyController*& GetHidyController() { return HidyController; }

private:

	bool CanSprint() const;

	//--------------- Movement bindings -----------------
	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);
	void LookGamepad(const FInputActionValue& Value);

	void WalkToggle(const FInputActionValue& Value);

	void Sprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);

	void TryCrouch(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void RPC_Server_UpdateInputState(const FPlayerInputState State);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};

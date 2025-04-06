// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Anims/Gait.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class HIDY_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

	class AHidyController* Controller = nullptr;
	UCharacterMovementComponent* Movement = nullptr;

	class UPreCMCTick* PreTick = nullptr;

protected:
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

	constexpr EGait GetGait() const { return Gait; }
	constexpr void SetGait(const EGait newGait) { Gait = newGait; }

private:

	bool CanSprint() const;
};

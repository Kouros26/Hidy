// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "HidyController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Runtime/AnimGraphRuntime/Public/KismetAnimationLibrary.h>

#include "PreCMCTick.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PreTick = CreateDefaultSubobject<UPreCMCTick>(TEXT("PreTick"));

	StrafeSpeedMapCurve.UpdateOrAddKey(0.0f, 0.0f);
	StrafeSpeedMapCurve.UpdateOrAddKey(45.0f, 0.0f);
	StrafeSpeedMapCurve.UpdateOrAddKey(80.0f, 1.0f);
	StrafeSpeedMapCurve.UpdateOrAddKey(100.0f, 1.0f);
	StrafeSpeedMapCurve.UpdateOrAddKey(130.0f, 2.0f);
	StrafeSpeedMapCurve.UpdateOrAddKey(180.0f, 2.0f);
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	Controller = Cast<AHidyController>(NewController);
	PreTick->SetController(Controller);

	if (GetLocalRole() != ROLE_AutonomousProxy)
		return;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Movement = GetCharacterMovement();

	Movement->AddTickPrerequisiteComponent(PreTick);
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool APlayerCharacter::CanSprint() const
{
	const FVector vec = IsLocallyControlled() ? GetPendingMovementInputVector() : Movement->GetCurrentAcceleration();
	const FRotator rot = vec.Rotation();

	const float yawDifference = FMath::Abs(FRotator::NormalizeAxis(GetActorRotation().Yaw - rot.Yaw));

	const bool facing = Movement->bOrientRotationToMovement ? true : yawDifference < 50.0f;

	return Controller->GetInputState().bWantsToSprint && facing;
}

EGait APlayerCharacter::GetDesiredGait() const
{
	if (CanSprint())
	{
		return EGait::SPRINT;
	}

	if (Controller->GetInputState().bWantsToWalk)
	{
		return EGait::WALK;
	}

	return EGait::RUN;
}

float APlayerCharacter::CalculateMaxAcceleration() const
{
	const float speedXY = Movement->Velocity.Size2D();

	const float mappedAcceleration = FMath::GetMappedRangeValueClamped(
		FVector2D(300.0f, 700.0f),
		FVector2D(800.0f, 300.0f),
		speedXY
	);

	switch (Gait)
	{
	case EGait::WALK:  
		return 800.0f; // Walk
	case EGait::RUN:  
		return 800.0f; // Run
	case EGait::SPRINT:  
		return mappedAcceleration; // Sprint
	default: 
		return 800.0f; // Default case
	}
}

float APlayerCharacter::CalculateMaxSpeed() const
{
	const float deviation = FMath::Abs(UKismetAnimationLibrary::CalculateDirection(Movement->Velocity, GetActorRotation()));

	const float strafeSpeedMap = Movement->bUseControllerDesiredRotation ? StrafeSpeedMapCurve.Eval(deviation) : 0.0f;

	FVector3f speeds{};

	switch (Gait)
	{
	case EGait::WALK:
		speeds = WalkSpeeds;
		break;

	case EGait::RUN:
		speeds = RunSpeeds;
		break;

	case EGait::SPRINT:
		speeds = SprintSpeeds;
		break;
	}

	const float first = FMath::GetMappedRangeValueClamped(
		FVector2D(0.0f, 1.0f),
		FVector2D(speeds.X, speeds.Y),
		strafeSpeedMap
	);

	const float sec = FMath::GetMappedRangeValueClamped(
		FVector2D(1.0f, 2.0f),
		FVector2D(speeds.Y, speeds.Z),
		strafeSpeedMap
	);

	UE_LOG(LogTemp, Log, TEXT("%f"), strafeSpeedMap < 1.0f ? first : sec);

	return strafeSpeedMap < 1.0f ? first : sec;
}

float APlayerCharacter::CalculateMaxCrouchSpeed() const
{
	const float deviation = FMath::Abs(UKismetAnimationLibrary::CalculateDirection(Movement->Velocity, GetActorRotation()));

	const float strafeSpeedMap = Movement->bOrientRotationToMovement ? 0.0f : StrafeSpeedMapCurve.Eval(deviation);

	const float first = FMath::GetMappedRangeValueClamped(
		FVector2D(0.0f, 1.0f),
		FVector2D(CrouchSpeeds.X, CrouchSpeeds.Y),
		strafeSpeedMap
	);

	const float sec = FMath::GetMappedRangeValueClamped(
		FVector2D(1.0f, 2.0f),
		FVector2D(CrouchSpeeds.Y, CrouchSpeeds.Z),
		strafeSpeedMap
	);

	return strafeSpeedMap < 1.0f ? first : sec;
}

float APlayerCharacter::CalculateBreakingDeceleration() const
{
	return GetPendingMovementInputVector() != FVector::ZeroVector ? 500 : 2000;
}

float APlayerCharacter::CalculateGroundFriction() const
{
	const float speedXY = Movement->Velocity.Size2D();

	const float mappedAcceleration = FMath::GetMappedRangeValueClamped(
		FVector2D(0.0f, 500.0f),
		FVector2D(5.0f, 3.0f),
		speedXY
	);

	switch (Gait)
	{
	case EGait::WALK:
		return 5.0f;

	case EGait::RUN:
		return 5.0f;

	case EGait::SPRINT:
		return mappedAcceleration;
	}

	return mappedAcceleration;
}

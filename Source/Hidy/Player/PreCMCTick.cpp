// Fill out your copyright notice in the Description page of Project Settings.


#include "PreCMCTick.h"

#include "HidyController.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UPreCMCTick::UPreCMCTick()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPreCMCTick::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<APlayerCharacter>(GetOwner());
	Movement = Player->GetCharacterMovement();
}


// Called every frame
void UPreCMCTick::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateRotationPreTick();
	UpdateMovementPreTick();
}

void UPreCMCTick::UpdateRotationPreTick() const
{
	FPlayerInputState state = Player->GetInputState();

	if (state.bWantsToStrafe || state.bWantsToAim)
	{
		
		Movement->bUseControllerDesiredRotation = true;
		Movement->bOrientRotationToMovement = false;
	}

	else
	{
;		Movement->bUseControllerDesiredRotation = true;
		Movement->bOrientRotationToMovement = false;
	}

	if (Movement->IsFalling())
	{
		Movement->RotationRate = { 0.0, 0.0, 200.0 };
	}

	else
	{
		Movement->RotationRate = { -1.0, -1.0, -1.0 };
	}
}

void UPreCMCTick::UpdateMovementPreTick() const
{
	Player->SetGait(Player->GetDesiredGait());

	Movement->MaxAcceleration = Player->CalculateMaxAcceleration();
	Movement->BrakingDecelerationWalking = Player->CalculateBreakingDeceleration();
	Movement->GroundFriction = Player->CalculateGroundFriction();
	Movement->MaxWalkSpeed = Player->CalculateMaxSpeed();
	Movement->MaxWalkSpeedCrouched = Player->CalculateMaxCrouchSpeed();
}
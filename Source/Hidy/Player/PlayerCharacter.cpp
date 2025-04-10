// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include "HidyController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Runtime/AnimGraphRuntime/Public/KismetAnimationLibrary.h>

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PreCMCTick.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Hidy/Debug/Print.h"
#include "Net/UnrealNetwork.h"

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

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 150.0f;
	SpringArm->SetRelativeLocation({ 20.0f, 0.0f, 80.0f });
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Spotlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spotlight"));
	Spotlight->SetupAttachment(GetMesh(), "head");
	Spotlight->SetRelativeLocation({ 9.0f, 9.0f, 0.0f });
	Spotlight->SetRelativeRotation({ 0.0f, 0.0f, -90.0f });

	if (fps)
	{
		SpringArm->TargetArmLength = 0;
		SpringArm->SetupAttachment(GetMesh(), "head");
		SpringArm->SetRelativeLocation({ 20.0f, 12.0f, 0.0f });
		Camera->SetRelativeLocation({ -10.0f, 0.0f, 0.0f });
	}
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	HidyController = Cast<AHidyController>(NewController);

	if (GetLocalRole() != ROLE_AutonomousProxy)
		return;

}

void APlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

	HidyController = Cast<AHidyController>(GetController());
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Movement = GetCharacterMovement();
	Movement->AddTickPrerequisiteComponent(PreTick);
	Movement->bUseControllerDesiredRotation = true;
	Movement->bOrientRotationToMovement = false;

	if (!IsLocallyControlled())
		return;

	if (HidyController)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(HidyController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(HidyController->InputMapping, 0);
		}
	}

	if (fps)
	{
		SpringArm->TargetArmLength = 0;
		SpringArm->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "head");
		SpringArm->SetRelativeLocation({ 10.0f, 12.0f, 0.0f });
		Camera->SetRelativeLocation({ -10.0f, 0.0f, 0.0f });
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsLocallyControlled())
		return;

	const float angle = FMath::Abs(FMath::FindDeltaAngleDegrees(GetMesh()->GetBoneQuaternion("spine_05").Rotator().Yaw, Camera->GetComponentRotation().Yaw));

	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Cyan, FString::SanitizeFloat(GetMesh()->GetBoneQuaternion("spine_05").Rotator().Yaw));
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, FString::SanitizeFloat(Camera->GetComponentRotation().Yaw));
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::SanitizeFloat(angle));

	angle > deltaCamPlayerThreshold ? GetMesh()->SetVisibility(false) :
	GetMesh()->SetVisibility(true);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Bind Input Actions
		EnhancedInputComp->BindAction(HidyController->IA_Move, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComp->BindAction(HidyController->IA_Look, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComp->BindAction(HidyController->IA_LookGamepad, ETriggerEvent::Triggered, this, &APlayerCharacter::LookGamepad);
		EnhancedInputComp->BindAction(HidyController->IA_Walk, ETriggerEvent::Triggered, this, &APlayerCharacter::WalkToggle);
		EnhancedInputComp->BindAction(HidyController->IA_Sprint, ETriggerEvent::Triggered, this, &APlayerCharacter::Sprint);
		EnhancedInputComp->BindAction(HidyController->IA_Sprint, ETriggerEvent::Completed, this, &APlayerCharacter::StopSprint);
		EnhancedInputComp->BindAction(HidyController->IA_Crouch, ETriggerEvent::Triggered, this, &APlayerCharacter::TryCrouch);
	}
}

bool APlayerCharacter::CanSprint() const
{
	const FVector vec = IsLocallyControlled() ? GetPendingMovementInputVector() : Movement->GetCurrentAcceleration();
	const FRotator rot = vec.Rotation();

	const float yawDifference = FMath::Abs(FRotator::NormalizeAxis(GetActorRotation().Yaw - rot.Yaw));

	const bool facing = Movement->bOrientRotationToMovement ? true : yawDifference < 50.0f;

	return InputState.bWantsToSprint && facing;
}

EGait APlayerCharacter::GetDesiredGait() const
{
	if (CanSprint())
	{
		return EGait::SPRINT;
	}

	if (InputState.bWantsToWalk)
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

FPlayerInputState APlayerCharacter::GetInputState() const
{
	return InputState;
}

void APlayerCharacter::SetInputState(const FPlayerInputState Other)
{
	InputState = Other;
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D input = Value.Get<FVector2D>();

	AddMovementInput(GetActorRightVector() * input.X);
	AddMovementInput(GetActorForwardVector() * input.Y);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D input = Value.Get<FVector2D>();

	HidyController->AddYawInput(input.X);
	HidyController->AddPitchInput(input.Y);
}

void APlayerCharacter::LookGamepad(const FInputActionValue& Value)
{
	FVector2D input = Value.Get<FVector2D>();
	input *= GetWorld()->GetDeltaSeconds();

	HidyController->AddYawInput(input.X);
	HidyController->AddPitchInput(input.Y);
}

void APlayerCharacter::WalkToggle(const FInputActionValue& Value)
{
	if (InputState.bWantsToSprint)
		return;

	InputState.bWantsToWalk = !InputState.bWantsToWalk;
	RPC_Server_UpdateInputState(InputState);
}

void APlayerCharacter::Sprint(const FInputActionValue& Value)
{
	InputState.bWantsToSprint = true;
	InputState.bWantsToWalk = false;

	RPC_Server_UpdateInputState(InputState);
}

void APlayerCharacter::StopSprint(const FInputActionValue& Value)
{
	InputState.bWantsToSprint = false;

	RPC_Server_UpdateInputState(InputState);
}

void APlayerCharacter::TryCrouch(const FInputActionValue& Value)
{
	if (Movement->IsFalling())
		return;

	bIsCrouched ? UnCrouch() : Crouch();
}

void APlayerCharacter::RPC_Server_UpdateInputState_Implementation(const FPlayerInputState State)
{
	InputState = State;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, InputState);
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "HidyController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Net/UnrealNetwork.h"

void AHidyController::HidyController()
{
    bReplicates = true;
}

FPlayerInputState AHidyController::GetInputState() const
{
    return InputState;
}

void AHidyController::SetInputState(const FPlayerInputState Other)
{
    InputState = Other;
}

void AHidyController::BeginPlay()
{
	Super::BeginPlay();

    UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

    if (InputSubsystem)
    {
        InputSubsystem->AddMappingContext(InputMapping, 0);
    }
}

void AHidyController::SetupInputComponent()
{
	Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // Bind Input Actions
        EnhancedInputComp->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AHidyController::Move);
        EnhancedInputComp->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AHidyController::Look);
        EnhancedInputComp->BindAction(IA_Walk, ETriggerEvent::Triggered, this, &AHidyController::WalkToggle);
        EnhancedInputComp->BindAction(IA_Sprint, ETriggerEvent::Triggered, this, &AHidyController::Sprint);
        EnhancedInputComp->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &AHidyController::StopSprint);
        EnhancedInputComp->BindAction(IA_Crouch, ETriggerEvent::Triggered, this, &AHidyController::Crouch);
    }
}

void AHidyController::Move(const FInputActionValue& Value)
{
    APawn* pawn = GetPawn();
    FVector2D input = Value.Get<FVector2D>();

    pawn->AddMovementInput(pawn->GetActorRightVector() * input.X);
    pawn->AddMovementInput(pawn->GetActorForwardVector() * input.Y);
}

void AHidyController::Look(const FInputActionValue& Value)
{
    FVector2D input = Value.Get<FVector2D>();

    AddYawInput(input.X);
    AddPitchInput(input.Y);
}

void AHidyController::WalkToggle(const FInputActionValue& Value)
{
    if (InputState.bWantsToSprint)
        return;

	InputState.bWantsToWalk = !InputState.bWantsToWalk;
    RPC_Server_UpdateInputState(InputState);
}

void AHidyController::Sprint(const FInputActionValue& Value)
{
    InputState.bWantsToSprint = true;
    InputState.bWantsToWalk = false;

    RPC_Server_UpdateInputState(InputState);
}

void AHidyController::StopSprint(const FInputActionValue& Value)
{
    InputState.bWantsToSprint = false;

    RPC_Server_UpdateInputState(InputState);
}

void AHidyController::Crouch(const FInputActionValue& Value)
{
    ACharacter* Chara = GetCharacter();

    if (Chara->GetCharacterMovement()->IsFalling())
        return;

    Chara->bIsCrouched ? Chara->UnCrouch() : Chara->Crouch();
}

void AHidyController::RPC_Server_UpdateInputState/*_Implementation*/(const FPlayerInputState State)
{
    InputState = State;
}

void AHidyController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //DOREPLIFETIME(AHidyController, InputState);
}

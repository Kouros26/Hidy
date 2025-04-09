// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "Components/ActorComponent.h"
#include "PreCMCTick.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class HIDY_API UPreCMCTick : public UActorComponent
{
	GENERATED_BODY()

	class APlayerCharacter* Player;
	class UCharacterMovementComponent* Movement;

public:
	// Sets default values for this component's properties
	UPreCMCTick();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	void UpdateRotationPreTick() const;
	void UpdateMovementPreTick() const;
};

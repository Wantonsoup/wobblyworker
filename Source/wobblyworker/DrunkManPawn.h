// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DrunkManPawn.generated.h"


// Components //
class USphereComponent;
class USceneComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UEnhancedInputComponent;
 
// Input //
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class WOBBLYWORKER_API ADrunkManPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADrunkManPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	void Move(const FInputActionValue& Value);
	void StopMove(const FInputActionValue& Value);

	void ApplyMovement(float DeltaTime);
	void ApplyStumble(float DeltaTime);
	void UpdateBalance(float DeltaTime);
	void UpdateVisualLean(float DeltaTime);
	void FallOver();

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* PhysicsSphere;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* VisualRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USkeletalMeshComponent* DrunkManMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	FVector2D TargetInput;
	FVector2D CurrentInput;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MoveForce = 90000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MaxSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float InputResponse = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drunk Effects", meta = (AllowPrivateAccess = "true"))
	float WobbleStrength = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drunk Effects", meta = (AllowPrivateAccess = "true"))
	float WobbleSpeed = 4.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drunk Effects", meta = (AllowPrivateAccess = "true"))
	float LeanAmount = 0.03f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drunk Effects", meta = (AllowPrivateAccess = "true"))
	float MaxLeanAngle = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (AllowPrivateAccess = "true"))
	float Balance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (AllowPrivateAccess = "true"))
	float UnsafeSpeed = 850.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (AllowPrivateAccess = "true"))
	float BalanceLossRate = 18.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (AllowPrivateAccess = "true"))
	float BalanceRecoveryRate = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (AllowPrivateAccess = "true"))
	float LeanBalanceLossThreshold = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (AllowPrivateAccess = "true"))
	float LeanBalanceLossRate = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (AllowPrivateAccess = "true"))
	float FallOverLeanThreshold = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stumble", meta = (AllowPrivateAccess = "true"))
	float StumbleImpulse = 55000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stumble", meta = (AllowPrivateAccess = "true"))
	float StumbleInterval = 2.0f;

	float StumbleTimer = 0.0f;

	bool bHasFallen = false;
};

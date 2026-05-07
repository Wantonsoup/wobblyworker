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

UENUM(BlueprintType)
enum class EGetUpDirection : uint8
{
	None,
	Forward,
	Backward,
	
};


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

	UFUNCTION(BlueprintCallable)
	void FallOver();

	UFUNCTION(BlueprintCallable)
	void StartGetUp();

	UFUNCTION(BlueprintCallable)
	void FinishGetUp();


	UFUNCTION(BlueprintPure)
	FVector2D GetMovementVelocity() const;

	

	UFUNCTION(BlueprintPure)
	bool IsRagdolling() const;

	UFUNCTION(BlueprintPure)
	bool IsGettingUp() const;

	UFUNCTION(BlueprintPure)
	EGetUpDirection GetGetUpDirection() const;

	
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
	float MoveForce = 50000.0f; // Force applied to move the character, higher values = faster acceleration

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float MaxSpeed = 1000.0f; // Maximum speed the character can reach
	 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float InputResponse = 1.0f; // How quickly the character responds to input changes

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drunk Effects", meta = (AllowPrivateAccess = "true"))
	float WobbleStrength = 12.0f; // How much the character wobbles when idle

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drunk Effects", meta = (AllowPrivateAccess = "true"))
	float WobbleSpeed = 4.0f; // How fast the character wobbles when idle

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drunk Effects", meta = (AllowPrivateAccess = "true"))
	float LeanAmount = 0.03f; // How much the character leans based on movement input

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drunk Effects", meta = (AllowPrivateAccess = "true"))
	float MaxLeanAngle = 18.0f; // In degrees

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (AllowPrivateAccess = "true"))
	float Balance = 100.0f; // 0 = fallen, 100 = perfect balance

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (AllowPrivateAccess = "true"))
	float UnsafeSpeed = 850.0f; // Speed above which the character starts losing balance. 
								// This encourages players to move carefully rather than just spamming movement input.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (AllowPrivateAccess = "true"))
	float BalanceLossRate = 18.0f; // How quickly balance decreases when above UnsafeSpeed or leaning too much, higher values = faster loss

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (AllowPrivateAccess = "true"))
	float BalanceRecoveryRate = 8.0f; // How quickly balance recovers when below UnsafeSpeed and not leaning too much, higher values = faster recovery

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (AllowPrivateAccess = "true"))
	float LeanBalanceLossThreshold = 45.0f; // Lean angle (in degrees) above which the character starts losing balance. 
											// This encourages players to not just hold a movement direction and lean indefinitely, but to also adjust their movement and lean angle.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (AllowPrivateAccess = "true"))
	float LeanBalanceLossRate = 25.0f; // How quickly balance decreases based on how much the lean angle is above the LeanBalanceLossThreshold, higher values = faster loss

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balance", meta = (AllowPrivateAccess = "true"))
	float FallOverLeanThreshold = 90.0f; // Lean angle (in degrees) at which the character will immediately fall over, regardless of current balance. 
	// This is to prevent players from just accepting a low balance and leaning as much as they want until they eventually fall over, by giving them a clear limit.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stumble", meta = (AllowPrivateAccess = "true"))
	float StumbleImpulse = 55000.0f; // Impulse applied in a random direction at regular intervals to make the character feel more "drunk". Higher values = stronger stumbles

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stumble", meta = (AllowPrivateAccess = "true"))
	float StumbleInterval = 2.0f; // Time in seconds between stumble impulses. Lower values = more frequent stumbles

	float StumbleTimer = 0.0f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EGetUpDirection GetUpDirection = EGetUpDirection::None;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsRagdolling = false;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsGettingUp = false;

	FTimerHandle GetUpTimerHandle;

	bool bHasFallen = false;
};

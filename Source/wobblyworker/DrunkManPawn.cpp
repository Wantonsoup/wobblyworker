// Fill out your copyright notice in the Description page of Project Settings.
#include "DrunkManPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// Sets default values
ADrunkManPawn::ADrunkManPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PhysicsSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PhysicsSphere"));
	RootComponent = PhysicsSphere;

	PhysicsSphere->SetSphereRadius(45.0f);
	PhysicsSphere->SetSimulatePhysics(true);
	PhysicsSphere->SetEnableGravity(true);
	PhysicsSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PhysicsSphere->SetCollisionObjectType(ECC_Pawn);
	PhysicsSphere->SetLinearDamping(0.5f);
	PhysicsSphere->SetAngularDamping(1.5f);

	// Hide the real player sphere in-game.
	PhysicsSphere->SetHiddenInGame(true);

	VisualRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VisualRoot"));
	VisualRoot->SetupAttachment(PhysicsSphere);

	DrunkManMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("DrunkManMesh"));
	DrunkManMesh->SetupAttachment(VisualRoot);

	// The man is only visual. The sphere handles collision.
	DrunkManMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DrunkManMesh->SetSimulatePhysics(false);
	DrunkManMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -45.0f));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(PhysicsSphere);
	SpringArm->TargetArmLength = 500.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 6.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

}

// Called when the game starts or when spawned
void ADrunkManPawn::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();

		if (LocalPlayer)
		{
			UEnhancedInputLocalPlayerSubsystem* Subsystem =
				LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

			if (Subsystem && InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}

// Called every frame
void ADrunkManPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasFallen)
	{
		return;
	}

	ApplyMovement(DeltaTime);
	ApplyStumble(DeltaTime);
	UpdateBalance(DeltaTime);
	UpdateVisualLean(DeltaTime);
	
}

// Called to bind functionality to input
void ADrunkManPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!EnhancedInputComponent)
	{
		return;
	}

	if (MoveAction)
	{
		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Triggered,
			this,
			&ADrunkManPawn::Move
		);

		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Completed,
			this,
			&ADrunkManPawn::StopMove
		);
	}
}

void ADrunkManPawn::Move(const FInputActionValue& Value)
{
	TargetInput = Value.Get<FVector2D>();
}

void ADrunkManPawn::StopMove(const FInputActionValue& Value)
{
	TargetInput = FVector2D::ZeroVector;
}

void ADrunkManPawn::ApplyMovement(float DeltaTime)
{
	CurrentInput = FMath::Vector2DInterpTo(
		CurrentInput,
		TargetInput,
		DeltaTime,
		InputResponse
	);

	if (CurrentInput.IsNearlyZero())
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (!PlayerController)
	{
		return;
	}

	const FRotator ControlRotation = PlayerController->GetControlRotation();
	const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ForwardDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	const FVector RightDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	FVector MoveDirection =
		ForwardDirection * CurrentInput.Y +
		RightDirection * CurrentInput.X;

	MoveDirection.Normalize();

	const FVector CurrentVelocity = PhysicsSphere->GetPhysicsLinearVelocity();

	if (CurrentVelocity.Size() < MaxSpeed)
	{
		PhysicsSphere->AddForce(MoveDirection * MoveForce);
	}
}

void ADrunkManPawn::ApplyStumble(float DeltaTime)
{
	StumbleTimer += DeltaTime;

	if (StumbleTimer < StumbleInterval)
	{
		return;
	}

	StumbleTimer = 0.0f;

	const float StumbleChance = 1.0f - Balance / 100.0f;

	if (FMath::FRand() > StumbleChance)
	{
		return;
	}

	FVector RandomDirection(
		FMath::FRandRange(-1.0f, 1.0f),
		FMath::FRandRange(-1.0f, 1.0f),
		0.0f
	);

	RandomDirection.Normalize();

	PhysicsSphere->AddImpulse(RandomDirection * StumbleImpulse);
}

void ADrunkManPawn::UpdateBalance(float DeltaTime)
{
	const float Speed = PhysicsSphere->GetPhysicsLinearVelocity().Size();

	// Speed-based balance loss
	if (Speed > UnsafeSpeed)
	{
		Balance -= BalanceLossRate * DeltaTime;
	}
	else
	{
		Balance += BalanceRecoveryRate * DeltaTime;
	}

	// Get the visible man's lean angle
	const FRotator VisualRotation = VisualRoot->GetComponentRotation();

	const float Pitch = FMath::Abs(FRotator::NormalizeAxis(VisualRotation.Pitch));
	const float Roll = FMath::Abs(FRotator::NormalizeAxis(VisualRotation.Roll));

	const float LeanAngle = FMath::Max(Pitch, Roll);


	// Lean-based balance loss
	if (LeanAngle > LeanBalanceLossThreshold)
	{
		const float LeanAmountOverThreshold = LeanAngle - LeanBalanceLossThreshold;

		Balance -= LeanAmountOverThreshold * LeanBalanceLossRate * DeltaTime;
	}

	Balance = FMath::Clamp(Balance, 0.0f, 100.0f);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			1,
			0.0f,
			FColor::Green,
			FString::Printf(TEXT("Balance: %.1f | Lean: %.1f | Speed: %.1f"), Balance, LeanAngle, Speed)
		);
	}

	if (Balance <= 0.0f || LeanAngle >= FallOverLeanThreshold)
	{
		FallOver();
	}
}

void ADrunkManPawn::UpdateVisualLean(float DeltaTime)
{
	const FVector Velocity = PhysicsSphere->GetPhysicsLinearVelocity();

	const float ForwardLean = FMath::Clamp(
		Velocity.X * LeanAmount,
		-MaxLeanAngle,
		MaxLeanAngle
	);

	const float SideLean = FMath::Clamp(
		Velocity.Y * LeanAmount,
		-MaxLeanAngle,
		MaxLeanAngle
	);

	const float Time = GetWorld()->GetTimeSeconds();

	const float WobblePitch =
		FMath::Cos(Time * WobbleSpeed * 0.7f) * WobbleStrength;

	const float WobbleRoll =
		FMath::Sin(Time * WobbleSpeed) * WobbleStrength;

	const FRotator TargetWorldRotation(
		ForwardLean + WobblePitch,
		GetControlRotation().Yaw,
		-SideLean + WobbleRoll
	);

	const FRotator NewWorldRotation = FMath::RInterpTo(
		VisualRoot->GetComponentRotation(),
		TargetWorldRotation,
		DeltaTime,
		6.0f
	);

	VisualRoot->SetWorldRotation(NewWorldRotation);
}

void ADrunkManPawn::FallOver()
{
	bHasFallen = true;

	PhysicsSphere->AddImpulse(
		FVector(0.0f, 0.0f, 30000.0f) +
		(-GetActorForwardVector() * 40000.0f)
	);

	DrunkManMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	DrunkManMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DrunkManMesh->SetSimulatePhysics(true);

	UE_LOG(LogTemp, Warning, TEXT("FallOver called. Balance: %.1f, Speed: %.1f, Location Z: %.1f"),
		Balance,
		PhysicsSphere->GetPhysicsLinearVelocity().Size(),
		GetActorLocation().Z
	);

}


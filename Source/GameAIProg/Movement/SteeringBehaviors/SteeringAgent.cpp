// Fill out your copyright notice in the Description page of Project Settings.

#include "SteeringAgent.h"

#include "AIController.h"


// Sets default values
ASteeringAgent::ASteeringAgent()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASteeringAgent::BeginPlay()
{
	Super::BeginPlay();
}

void ASteeringAgent::BeginDestroy()
{
	Super::BeginDestroy();
}

// Called every frame
void ASteeringAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SteeringBehavior)
	{
		SteeringOutput output = SteeringBehavior->CalculateSteering(DeltaTime, *this);
		
		if (output.IsValid)
		{
			AddMovementInput(FVector{output.LinearVelocity, 0.f});
			
			//if (!IsAutoOrienting())
			{
				if (AAIController* AIController = Cast<AAIController>(GetController()))
				{
					const float DeltaYaw = FMath::Clamp(output.AngularVelocity, -1.0f, 1.0f) * GetMaxAngularSpeed() * DeltaTime;
					
					FRotator const currentRotation {GetActorForwardVector().ToOrientationRotator()};
					FRotator DeltaRotation = FRotator(0.f, DeltaYaw, 0.f);
					FRotator const DesiredRotation {currentRotation + DeltaRotation};
					
					if (!FMath::IsNearlyEqual(currentRotation.Yaw, DesiredRotation.Yaw))
					{
						AIController->SetControlRotation(DesiredRotation);
						FaceRotation(DesiredRotation);
					}
				}
			}
		}
		
		// float speedRotation{400.f};
		// float RotationAmount = output.AngularVelocity * DeltaTime * speedRotation;
		// AddActorLocalRotation(DeltaRotation);
	}
	
}

// Called to bind functionality to input
void ASteeringAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASteeringAgent::SetSteeringBehavior(ISteeringBehavior* NewSteeringBehavior)
{
	SteeringBehavior = NewSteeringBehavior;
}


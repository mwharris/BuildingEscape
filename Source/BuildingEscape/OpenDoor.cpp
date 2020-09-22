// Copyright Michael Harris 2020

#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

#define OUT 

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	CheckPressurePlate();
	FindAudioComponent();
	InitVars();
}

// Make sure we have a pressure plate volume set
void UOpenDoor::CheckPressurePlate() const
{
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the OpenDoor component but no pressure plate set!"), *GetOwner()->GetName());
	}
}

// Helper function to get an attached UAudioComponent
void UOpenDoor::FindAudioComponent()
{
	Audio = GetOwner()->FindComponentByClass<UAudioComponent>();
	if (!Audio) 
	{
		UE_LOG(LogTemp, Warning, TEXT("No audio component on %s"), *GetOwner()->GetName());
	}
}

// Initialize some starting values
void UOpenDoor::InitVars()
{
	FRotator ActorRotator = GetOwner()->GetActorRotation();
	InitialYaw = ActorRotator.Yaw;
	CurrentYaw = ActorRotator.Yaw;
	OpenAngle += ActorRotator.Yaw;
	DoorClosing = true;
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressurePlate) 
	{
		float TimeSeconds = GetWorld()->GetTimeSeconds();
		float CurrentMass = TotalMassOfActors();
		if (FMath::IsNearlyEqual(CurrentMass, RequiredMass, 0.1f)) 
		{
			OpenDoor(DeltaTime);
			DoorLastOpened = TimeSeconds;
			if (!DoorOpening) 
			{
				PlayDoorSound();
				DoorOpening = true;
				DoorClosing = false;
			}
		}
		else  if (TimeSeconds - DoorLastOpened > CloseDelay) 
		{
			CloseDoor(DeltaTime);
			if (!DoorClosing) 
			{
				PlayDoorSound();
				DoorClosing = true;
				DoorOpening = false;
			}
		}
	}
}

// Helper function to sum the mass of actors in the volume
float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;
	if (PressurePlate)
	{
		// Find all overlapping actors
		TArray<AActor*> OverlappingActors;
		PressurePlate->GetOverlappingActors(OUT OverlappingActors);
		// Sum their masses
		for (AActor* Actor : OverlappingActors) 
		{
			TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		}
	}
	return TotalMass;
}

// Open the door slowly
void UOpenDoor::OpenDoor(float DeltaTime)
{
	FRotator ActorRotator = GetOwner()->GetActorRotation();
	// Smooth exponential interpolation
	CurrentYaw = FMath::Lerp(ActorRotator.Yaw, OpenAngle, DeltaTime * OpenLerpTime);
	if ((OpenAngle - CurrentYaw) < 0.01f) 
	{
		CurrentYaw = OpenAngle;
	}
	ActorRotator.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(ActorRotator);
}

// Close the door forcefully
void UOpenDoor::CloseDoor(float DeltaTime)
{
	FRotator ActorRotator = GetOwner()->GetActorRotation();
	// True linear interpolation
	CurrentYaw = FMath::FInterpConstantTo(ActorRotator.Yaw, InitialYaw, DeltaTime, CloseSpeed);
	ActorRotator.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(ActorRotator);
}

void UOpenDoor::PlayDoorSound() const
{
	if (Audio) 
	{
		Audio->Play();
	}
}
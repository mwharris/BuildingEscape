// Copyright Michael Harris 2020

#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();

	// OpenActor = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	OpenActor = GetWorld()->GetFirstPlayerController()->GetPawn();

	FRotator ActorRotator = GetOwner()->GetActorRotation();
	InitialYaw = ActorRotator.Yaw;
	CurrentYaw = ActorRotator.Yaw;
	OpenAngle += ActorRotator.Yaw;
	
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the OpenDoor component but no pressure plate set!"), *GetOwner()->GetName());
	}
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressurePlate && OpenActor) 
	{
		float TimeSeconds = GetWorld()->GetTimeSeconds();
		if (PressurePlate->IsOverlappingActor(OpenActor)) 
		{
			OpenDoor(DeltaTime);
			DoorLastOpened = TimeSeconds;
		}
		else  if (TimeSeconds - DoorLastOpened > CloseDelay) 
		{
			CloseDoor(DeltaTime);
		}
	}
}

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

void UOpenDoor::CloseDoor(float DeltaTime)
{
	FRotator ActorRotator = GetOwner()->GetActorRotation();

	// True linear interpolation
	CurrentYaw = FMath::FInterpConstantTo(ActorRotator.Yaw, InitialYaw, DeltaTime, CloseSpeed);
	ActorRotator.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(ActorRotator);
}
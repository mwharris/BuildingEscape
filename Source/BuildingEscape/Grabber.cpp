// Copyright Michael Harris 2020

#include "Grabber.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

#define OUT 

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = GetWorld()->GetFirstPlayerController();
	FindPhysicsHandle();
	SetupInputComponent();
}

// Get a reference to our PhysicsHandle component
void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no PhysicsHandle attached!"), *GetOwner()->GetName());
	}
}

// Get our InputComponent and Bind functions to its input
void UGrabber::SetupInputComponent()
{
	Input = GetOwner()->FindComponentByClass<UInputComponent>();
	if (Input)
	{
		// Bind the Grab() and Release() functions to the IE_Pressed/IE_Released input event
		Input->BindAction("Grab", EInputEvent::IE_Pressed, this, &UGrabber::Grab);
		Input->BindAction("Grab", EInputEvent::IE_Released, this, &UGrabber::Release);
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no UInputComponent!"), *GetOwner()->GetName());
	}
}

// Called when we hold/press the Grab button 
void UGrabber::Grab()
{
	FHitResult Hit = GetFirstPhysicsBodyWithinReach();
	if (Hit.GetActor()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Grabbed %s!"), *Hit.GetActor()->GetName());
		PhysicsHandle->GrabComponentAtLocation(Hit.GetComponent(), NAME_None, Hit.Location);
	}
}

// Called when we release the Grab button
void UGrabber::Release()
{
	if (PhysicsHandle->GrabbedComponent) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Released %s!"), *PhysicsHandle->GetName());
		PhysicsHandle->ReleaseComponent();
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Get the player's view point location and rotation
	PlayerController->GetPlayerViewPoint(OUT ViewLocation, OUT ViewRotation);

	// If our Physics handle is grabbing a component
	if (PhysicsHandle->GrabbedComponent) 
	{
		// Set the grabbed component's location
		PhysicsHandle->SetTargetLocation(GetLineTraceEnd());
	}
}

// Raycast and return the first physics body we hit
FHitResult UGrabber::GetFirstPhysicsBodyWithinReach() const
{
	FVector LineTraceEnd = GetLineTraceEnd();

	// Draw a line so we can visualize in-editor
	DrawDebugLine(
		GetWorld(),
		ViewLocation,
		LineTraceEnd,
		FColor(0, 255, 0),
		false,
		0.f,
		0,
		5.f
	);

	// Perform the raycast
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	GetWorld()->LineTraceSingleByObjectType(
		Hit, 
		ViewLocation, 
		LineTraceEnd, 
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	return Hit;
}

// Calculate our raycast's endpoint based on view location and rotation
FVector UGrabber::GetLineTraceEnd() const
{
	FVector RaycastDirection = ViewRotation.Vector();
	FVector LineTraceEnd = ViewLocation + (RaycastDirection * Reach);
	return LineTraceEnd;
}
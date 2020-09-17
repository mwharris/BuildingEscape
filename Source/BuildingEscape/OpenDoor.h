// Copyright Michael Harris 2020

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "OpenDoor.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void OpenDoor(float DeltaTima);
	void CloseDoor(float DeltaTima);

private:
	AActor* OpenActor;
	float InitialYaw;
	float CurrentYaw;
	float DoorLastOpened = 0.f;

	UPROPERTY(EditAnywhere)
	ATriggerVolume* PressurePlate;

	UPROPERTY(EditAnywhere)
	float OpenAngle = 90.0f;

	UPROPERTY(EditAnywhere)
	float CloseDelay = 0.5f;

	UPROPERTY(EditAnywhere)
	float OpenLerpTime = 0.8f;

	UPROPERTY(EditAnywhere)
	float CloseSpeed = 150.f;

};

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
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CheckPressurePlate() const;
	void InitVars();
	void FindAudioComponent();
	void OpenDoor(float DeltaTima);
	void CloseDoor(float DeltaTima);
	float TotalMassOfActors() const;
	void PlayDoorSound() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	float InitialYaw;
	float CurrentYaw;
	float DoorLastOpened = 0.f;
	bool DoorOpening = false;
	bool DoorClosing = false;

	UPROPERTY(EditAnywhere)
	ATriggerVolume* PressurePlate = nullptr;

	UPROPERTY(EditAnywhere)
	float OpenAngle = 90.0f;

	UPROPERTY(EditAnywhere)
	float CloseDelay = 0.5f;

	UPROPERTY(EditAnywhere)
	float OpenLerpTime = 0.8f;

	UPROPERTY(EditAnywhere)
	float CloseSpeed = 150.f;

	UPROPERTY(EditAnywhere)
	float RequiredMass = 50.f;

	UPROPERTY()
	UAudioComponent* Audio = nullptr;
};

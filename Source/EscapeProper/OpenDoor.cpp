// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

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

	InitialZ = GetOwner() -> GetActorLocation().Z;
	CurrentZ = InitialZ;

	FindPressurePlate();
	FindAudioComponent();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(TotalMassOfActors() >= MassRequirement)
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld() -> GetTimeSeconds();
	}
	else
	{
		if(GetWorld() -> GetTimeSeconds() - DoorLastOpened >= DoorCloseDelay)
			CloseDoor(DeltaTime);
	}
	
	
}

void UOpenDoor::OpenDoor(float DeltaTime)
{
	CurrentZ = FMath::Lerp(CurrentZ, TargetZ, DeltaTime * DoorOpenFactor);
	FVector DoorLocation = GetOwner() -> GetActorLocation();
	DoorLocation.Z = CurrentZ;
	GetOwner() -> SetActorLocation(DoorLocation);

	CloseDoorSound = false;
	if(!AudioComponent) {return;}
	if(!OpenDoorSound)
	{
		AudioComponent -> Play();
		OpenDoorSound = true;
	}
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	CurrentZ = FMath::Lerp(CurrentZ, InitialZ, DeltaTime * DoorCloseFactor);
	FVector DoorLocation = GetOwner() -> GetActorLocation();
	DoorLocation.Z = CurrentZ;
	GetOwner() -> SetActorLocation(DoorLocation);

	OpenDoorSound = false;
	if(!AudioComponent) {return;}
	if(!CloseDoorSound)
	{
		AudioComponent -> Play();
		CloseDoorSound = true;
	}
}

float UOpenDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	// Find all Actors inside trigger
	TArray<AActor*> OverlappingActors;

	if(!PressurePlate) {return TotalMass;}
	PressurePlate -> GetOverlappingActors(OUT OverlappingActors);
	
	// Add their mass
	for(AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor -> FindComponentByClass<UPrimitiveComponent>() -> GetMass();
	}

	return TotalMass;
}

void UOpenDoor::FindAudioComponent()
{
	AudioComponent = GetOwner() -> FindComponentByClass<UAudioComponent>();

	if(!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("There's no Audio Componenton %s"), *GetOwner() -> GetName());
	}

}

void UOpenDoor::FindPressurePlate()
{
	if(!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("There is no volume assigned to %s!"), *GetOwner() -> GetName());
	}
}
// Fill out your copyright notice in the Description page of Project Settings.
#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"


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

	FindPhysicsHandle();
	SetupInputComponent();	
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner() -> FindComponentByClass<UInputComponent>();	
	if(InputComponent)
	{
		InputComponent -> BindAction(TEXT("Grab"), IE_Pressed, this, &UGrabber::Grab);
		InputComponent -> BindAction(TEXT("Grab"), IE_Released, this, &UGrabber::Release);
	}	
}

void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner() -> FindComponentByClass<UPhysicsHandleComponent>();
	
	// Checking for physics handle
	if(!PhysicsHandle)
	{
		UE_LOG(LogTemp, Error, TEXT("No physics handle found on: %s"), *GetOwner() -> GetName());
	}
}

void UGrabber::Grab()
{
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();
	AActor* ActorHit = HitResult.GetActor();

	if(ActorHit)
	{
		if(!PhysicsHandle) {return;}
		PhysicsHandle -> GrabComponentAtLocation
		(
			ComponentToGrab,
			NAME_None,
			GetPlayersReach()
		);
	}
}

void UGrabber::Release()
{
	if(!PhysicsHandle) {return;}
	PhysicsHandle -> ReleaseComponent();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!PhysicsHandle) {return;}
	// If the physics handle is attached
	if(PhysicsHandle -> GrabbedComponent)
	{
		// Move the object with the player
		PhysicsHandle -> SetTargetLocation(GetPlayersReach());
	}
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	// DrawDebugLine(
	// 	GetWorld(),
	// 	GetPlayersWorldPosition(),
	// 	GetPlayersReach(),
	// 	FColor(255, 0, 255),
	// 	false,
	// 	0.f,
	// 	0,
	// 	5.f
	// );

	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());
	FHitResult Hit;

	GetWorld() -> LineTraceSingleByObjectType(
		OUT Hit,
		GetPlayersWorldPosition(),
		GetPlayersReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	return Hit;	
}

FVector UGrabber::GetPlayersReach() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld() -> GetFirstPlayerController() -> GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation + Reach * PlayerViewPointRotation.Vector();
}

FVector UGrabber::GetPlayersWorldPosition() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld() -> GetFirstPlayerController() -> GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation;
}

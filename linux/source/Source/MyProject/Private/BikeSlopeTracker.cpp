#include "BikeSlopeTracker.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABikeSlopeTracker::ABikeSlopeTracker()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABikeSlopeTracker::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ABikeSlopeTracker::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (BikeActor)
    {
        float SlopeAngle = CalculateSlopeAngle();
        UE_LOG(LogTemp, Warning, TEXT("Current Slope Angle: %f"), SlopeAngle);
    }
}

float ABikeSlopeTracker::CalculateSlopeAngle()
{
    if (!BikeActor)
    {
        return 0.0f;
    }

    // Get the forward vector of the bike
    FVector ForwardVector = BikeActor->GetActorForwardVector();

    // Get the up vector of the bike
    FVector UpVector = BikeActor->GetActorUpVector();

    // Project the forward vector onto the ground plane (X-Y plane)
    FVector ForwardOnGround = FVector(ForwardVector.X, ForwardVector.Y, 0.0f);
    ForwardOnGround.Normalize();

    // Calculate the angle between the forward vector and its projection on the ground plane
    float DotProduct = FVector::DotProduct(ForwardVector, ForwardOnGround);
    float SlopeAngle = FMath::Acos(DotProduct);

    // Convert radians to degrees
    SlopeAngle = FMath::RadiansToDegrees(SlopeAngle);

    return SlopeAngle;
}

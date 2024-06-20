#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BikeSlopeTracker.generated.h"

UCLASS()
class MYPROJECT_API ABikeSlopeTracker : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ABikeSlopeTracker();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // The bike actor to track
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bike")
    AActor* BikeActor;

private:
    // Function to calculate the slope angle of the bike
    float CalculateSlopeAngle();
};

#include "HelloWorldActor.h"
#include "Engine/Engine.h"

// Sets default values
AHelloWorldActor::AHelloWorldActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AHelloWorldActor::BeginPlay()
{
    Super::BeginPlay();

    // Ekrana "Hello World" yazdýr
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Hello!!!"));
    }
}

// Called every frame
void AHelloWorldActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

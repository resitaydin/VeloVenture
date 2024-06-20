#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HelloWorldActor.generated.h"

UCLASS()
class MYPROJECT_API AHelloWorldActor : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AHelloWorldActor();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;
};

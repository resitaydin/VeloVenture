
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "ESP32Actor.generated.h"

UCLASS()
class MYPROJECT_API AESP32Actor : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AESP32Actor();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    void SendHttpPostRequest();

    void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};

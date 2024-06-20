#include "ESP32Actor.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

// Sets default values
AESP32Actor::AESP32Actor()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AESP32Actor::BeginPlay()
{
    Super::BeginPlay();

    SendHttpPostRequest();
}

// Called every frame
void AESP32Actor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AESP32Actor::SendHttpPostRequest()
{
    FString Url = TEXT("http://192.168.187.71/post");
    FString Content = TEXT("{\"example_key\":\"example_value\"}");

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->OnProcessRequestComplete().BindUObject(this, &AESP32Actor::OnResponseReceived);
    Request->SetURL(Url);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetContentAsString(Content);
    Request->ProcessRequest();
}

void AESP32Actor::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (bWasSuccessful && Response.IsValid())
    {
        FString ResponseContent = Response->GetContentAsString();
        UE_LOG(LogTemp, Warning, TEXT("Response received: %s"), *ResponseContent);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Request failed"));
    }
}

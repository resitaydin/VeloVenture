#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <termios.h>
#include "serial_com.generated.h"

UCLASS(BlueprintType, Category = "Communication Serial")
class MYPROJECT_API ASerialCom : public AActor
{
    GENERATED_BODY()
    
public:    
    ASerialCom();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category="Serial")
    bool OpenSerialPort();

    UFUNCTION(BlueprintCallable, Category="Serial")
    bool IsSerialPortOpen() const;

    UFUNCTION(BlueprintCallable, Category="Serial")
    void CloseSerialPort();

    UFUNCTION(BlueprintCallable, Category="Serial")
    FString ReadLine();
    
    UFUNCTION(BlueprintCallable, Category="Serial")
    void Flush();

    UFUNCTION(BlueprintCallable, Category="Serial")
    FString ReadString();

    UFUNCTION(BlueprintCallable, Category="Serial")
    void Print(const FString& Message);

    UFUNCTION(BlueprintCallable, Category="Serial")
    void Println(const FString& Message);

    UFUNCTION(BlueprintCallable, Category="Serial")
    static ASerialCom* OpenComPortWithFlowControl(bool& bOpened, int32 Port = 1, int32 BaudRate = 9600, bool DTR = true, bool RTS = true);

    UFUNCTION(BlueprintCallable, Category="Serial")
    bool OpenWFC(int32 Port, int32 BaudRate, bool DTR, bool RTS);

private:
    int32 SerialPort;
    struct termios Tty;
    const char* PortName;
};


#include "serial_com.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <iostream>

ASerialCom::ASerialCom()
{
    PrimaryActorTick.bCanEverTick = true;
    SerialPort = -1;
    PortName = "/dev/ttyUSB0"; // Set your port name here
}

void ASerialCom::BeginPlay()
{
    Super::BeginPlay();
}

void ASerialCom::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

bool ASerialCom::OpenSerialPort()
{
    SerialPort = open(PortName, O_RDWR | O_NOCTTY | O_SYNC);
    if (SerialPort < 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Error %d from open: %s"), errno, ANSI_TO_TCHAR(strerror(errno)));
        return false;
    }

    if (tcgetattr(SerialPort, &Tty) != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Error %d from tcgetattr: %s"), errno, ANSI_TO_TCHAR(strerror(errno)));
        CloseSerialPort(); // Close the port if tcgetattr fails
        return false;
    }

    cfsetospeed(&Tty, B115200);
    cfsetispeed(&Tty, B115200);

    Tty.c_cflag = (Tty.c_cflag & ~CSIZE) | CS8;
    Tty.c_iflag &= ~IGNBRK;
    Tty.c_lflag = 0;
    Tty.c_oflag = 0;
    Tty.c_cc[VMIN] = 1;  // Adjust this for non-blocking read
    Tty.c_cc[VTIME] = 1;  // Adjust this for non-blocking read

    Tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    Tty.c_cflag |= (CLOCAL | CREAD);
    Tty.c_cflag &= ~(PARENB | PARODD);
    Tty.c_cflag &= ~CSTOPB;

#ifdef CRTSCTS
    Tty.c_cflag &= ~CRTSCTS;
#elif defined(CRTS_IFLOW) && defined(CCTS_OFLOW)
    Tty.c_cflag &= ~(CRTS_IFLOW | CCTS_OFLOW);
#endif

    if (tcsetattr(SerialPort, TCSANOW, &Tty) != 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Error %d from tcsetattr: %s"), errno, ANSI_TO_TCHAR(strerror(errno)));
        CloseSerialPort(); // Close the port if tcsetattr fails
        return false;
    }

    // Flush serial port buffers
    tcflush(SerialPort, TCIOFLUSH);

    return true;
}

void ASerialCom::Flush()
{
    if (SerialPort < 0) return;

    char Buffer[8192];
    ssize_t bytesRead;
    
    do {
        bytesRead = read(SerialPort, Buffer, sizeof(Buffer));
    } while (bytesRead > 0);
}

bool ASerialCom::IsSerialPortOpen() const
{
    return (SerialPort >= 0);
}

void ASerialCom::CloseSerialPort()
{
    if (SerialPort >= 0)
    {
        close(SerialPort);
        SerialPort = -1;
    }
}

FString ASerialCom::ReadLine() {
    char Buffer[4096];
    memset(Buffer, 0, sizeof(Buffer));
    char* BufPtr = Buffer;
    char C;

    fd_set set;
    struct timeval timeout;

    FD_ZERO(&set);
    FD_SET(SerialPort, &set);	

    timeout.tv_sec = 0;  // Set to 0 to avoid blocking
    timeout.tv_usec = 5000; // 500 ms

    while (true) {
        int rv = select(SerialPort + 1, &set, NULL, NULL, &timeout);
        if (rv == -1) {
            UE_LOG(LogTemp, Error, TEXT("Error %d from select: %s"), errno, ANSI_TO_TCHAR(strerror(errno)));
            return FString();
        } else if (rv == 0) {
            UE_LOG(LogTemp, Error, TEXT("ReadLine timeout"));
            break;
        } else {
            ssize_t bytesRead = read(SerialPort, &C, 1);
            if (bytesRead < 0) {
                UE_LOG(LogTemp, Error, TEXT("Error %d from read: %s"), errno, ANSI_TO_TCHAR(strerror(errno)));
                return FString();
            }
            if (bytesRead == 0 || C == '\n' || BufPtr >= Buffer + sizeof(Buffer) - 1) {
                break;
            }
            *BufPtr++ = C;
        }
    }
    *BufPtr = '\0';
    UE_LOG(LogTemp, Log, TEXT("Buffer Content (raw): %s"), UTF8_TO_TCHAR(Buffer));
    return FString(UTF8_TO_TCHAR(Buffer));
}

FString ASerialCom::ReadString()
{
    char Buffer[256];
    memset(Buffer, 0, sizeof(Buffer));
    ssize_t bytesRead = read(SerialPort, Buffer, sizeof(Buffer) - 1);
    if (bytesRead < 0) {
        UE_LOG(LogTemp, Error, TEXT("Error %d from read: %s"), errno, ANSI_TO_TCHAR(strerror(errno)));
        return FString();
    }
    Buffer[bytesRead] = '\0';
    return FString(ANSI_TO_TCHAR(Buffer));
}

void ASerialCom::Print(const FString& Message)
{
    std::string Msg = TCHAR_TO_ANSI(*Message);
    ssize_t bytesWritten = write(SerialPort, Msg.c_str(), Msg.length());
    if (bytesWritten < 0) {
        UE_LOG(LogTemp, Error, TEXT("Error %d from write: %s"), errno, ANSI_TO_TCHAR(strerror(errno)));
    }
}

void ASerialCom::Println(const FString& Message)
{
    Print(Message);
    ssize_t bytesWritten = write(SerialPort, "\n", 1);
    if (bytesWritten < 0) {
        UE_LOG(LogTemp, Error, TEXT("Error %d from write: %s"), errno, ANSI_TO_TCHAR(strerror(errno)));
    }
}

ASerialCom* ASerialCom::OpenComPortWithFlowControl(bool& bOpened, int32 Port, int32 BaudRate, bool DTR, bool RTS)
{
    ASerialCom* Serial = NewObject<ASerialCom>();
    bOpened = Serial->OpenWFC(Port, BaudRate, DTR, RTS);
    return Serial;
}

bool ASerialCom::OpenWFC(int32 Port, int32 BaudRate, bool DTR, bool RTS) {
    SerialPort = open(PortName, O_RDWR | O_NOCTTY | O_SYNC);
    if (SerialPort < 0) {
        UE_LOG(LogTemp, Error, TEXT("Error %d from open: %s"), errno, ANSI_TO_TCHAR(strerror(errno)));
        return false;
    }

    if (tcgetattr(SerialPort, &Tty) != 0) {
        UE_LOG(LogTemp, Error, TEXT("Error %d from tcgetattr: %s"), errno, ANSI_TO_TCHAR(strerror(errno)));
        CloseSerialPort();
        return false;
    }

    cfsetospeed(&Tty, B115200);
    cfsetispeed(&Tty, B115200);

    Tty.c_cflag = (Tty.c_cflag & ~CSIZE) | CS8;
    Tty.c_iflag &= ~IGNBRK;
    Tty.c_lflag = 0;
    Tty.c_oflag = 0;
    Tty.c_cc[VMIN] = 1;  // Adjust this for non-blocking read
    Tty.c_cc[VTIME] = 1;  // Adjust this for non-blocking read

    Tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    Tty.c_cflag |= (CLOCAL | CREAD);
    Tty.c_cflag &= ~(PARENB | PARODD);
    Tty.c_cflag &= ~CSTOPB;

    int status;
    ioctl(SerialPort, TIOCMGET, &status);

    if (DTR) {
        status |= TIOCM_DTR;
    } else {
        status &= ~TIOCM_DTR;
    }

    if (RTS) {
        status |= TIOCM_RTS;
    } else {
        status &= ~TIOCM_RTS;
    }

    ioctl(SerialPort, TIOCMSET, &status);

    if (tcsetattr(SerialPort, TCSANOW, &Tty) != 0) {
        UE_LOG(LogTemp, Error, TEXT("Error %d from tcsetattr: %s"), errno, ANSI_TO_TCHAR(strerror(errno)));
        CloseSerialPort();
        return false;
    }

    tcflush(SerialPort, TCIOFLUSH);
    return true;
}


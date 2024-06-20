#include "FileReader.h"

#if PLATFORM_WINDOWS
#include "HAL/PlatformFilemanager.h"
#elif PLATFORM_LINUX
#include "HAL/PlatformFileManager.h" // Correct name and path
#else
#include "HAL/PlatformFileManager.h" // Fallback to general path
#endif
bool UFileReader::ReadFileWithPath(FString FilePath, FString& Result)
{
    if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
    {
        return FFileHelper::LoadFileToString(Result, *FilePath);
    }
    return false;
}

bool UFileReader::ReadFileWithoutPath(FString& Result)
{
    FString WorkingDirectory = FPaths::ConvertRelativePathToFull(TEXT(""));
    FString ParentDirectory = FPaths::GetPath(WorkingDirectory);
    ParentDirectory = FPaths::GetPath(ParentDirectory); // Navigate up one level
    ParentDirectory = FPaths::GetPath(ParentDirectory); // Navigate up another level
    ParentDirectory = FPaths::GetPath(ParentDirectory); // Navigate up another level
    FString FilePath = FPaths::Combine(ParentDirectory, TEXT("CurLogin.txt"));

    if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath))
    {
        return FFileHelper::LoadFileToString(Result, *FilePath);
    }
    else
    {
        Result = FilePath; // Set Result to the attempted file path
    }
    return false;
}




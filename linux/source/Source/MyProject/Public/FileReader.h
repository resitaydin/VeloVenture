// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FileReader.generated.h"

UCLASS()
class MYPROJECT_API UFileReader : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "FileIO")
    static bool ReadFileWithPath(FString FilePath, FString& Result);

    UFUNCTION(BlueprintCallable, Category = "FileIO")
    static bool ReadFileWithoutPath(FString& Result);
};

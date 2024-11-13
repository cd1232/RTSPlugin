// Craig Duthie 2023

// This Include
#include "RTSOrderTargetData.h"

// Engine Includes
#include <GameFramework/Actor.h>


FRTSOrderTargetData::FRTSOrderTargetData()
    : Actor(nullptr)
    , Location(FVector::ZeroVector)
    , TargetTags(FGameplayTagContainer::EmptyContainer)
{
}

FRTSOrderTargetData::FRTSOrderTargetData(AActor* InActor, const FVector InLocation,
    const FGameplayTagContainer& InTargetTags)
    : Actor(InActor)
    , Location(InLocation)
    , TargetTags(InTargetTags)
{
}

FString FRTSOrderTargetData::ToString() const
{
    FString s;
    s += TEXT("(");
    s += TEXT("Actor: ");
    if (Actor != nullptr)
    {
        s += Actor->GetName();
    }

    else
    {
        s += FName().ToString();
    }

    s += TEXT(", Location: ");
    s += Location.ToString();

    s += TEXT(", Tags:( ");
    s += TargetTags.ToString();
    s += TEXT(")");
    s += TEXT(")");

    return s;
}


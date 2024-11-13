// Craig Duthie 2023

#pragma once

// Engine Includes
#include <GameplayTagContainer.h>

// Generated Include
#include "RTSOrderTargetData.generated.h"

class AActor;

/** Targeting data for an order. */
USTRUCT(BlueprintType)
struct RTSPLUGIN_API FRTSOrderTargetData
{
    GENERATED_BODY()

    FRTSOrderTargetData();
    FRTSOrderTargetData(AActor* InActor, const FVector InLocation, const FGameplayTagContainer& InTargetTags);

    /** The target actor. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    AActor* Actor;

    /** The target location. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FVector Location;

    /** The target tags. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FGameplayTagContainer TargetTags;

    /**
     * Get a textual representation of this order target data.
     * @return A string describing the order target data.
     */
    FString ToString() const;
};

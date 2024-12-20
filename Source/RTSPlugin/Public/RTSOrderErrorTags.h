#pragma once

#include "GameplayTagContainer.h"

#include "RTSOrderErrorTags.generated.h"

/**
 * Encloses collections of missing, blocking and error tags that describe why an order cannot be issued.
 */
USTRUCT(BlueprintType)
struct RTSPLUGIN_API FRTSOrderErrorTags
{
    GENERATED_BODY()

    /** List of required tags that are missing. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer MissingTags;

    /** List of tags that are blocking the execution of the order. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer BlockingTags;

    /** List of additional error tags that describe additional missing requirements for the execution of the order. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer ErrorTags;

    /** Whether any tags have been specified, or not. */
    bool IsEmpty() const;

    /**
     * Get a textual representation of this order.
     * @return A string describing the order.
     */
    FString ToString() const;
};
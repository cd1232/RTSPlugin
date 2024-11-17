// Craig Duthie 2023

#pragma once

// Engine Includes
#include <Engine/DataAsset.h>
#include <GameplayTagContainer.h>
#include <BehaviorTree/BehaviorTree.h>

// Generated Includes
#include "RTSOrderData.generated.h"

class UBehaviorTree;
class UBlackboardData;
class URTSOrder;
class URTSGameplayAbility;

/**
 * 
 */
USTRUCT(BlueprintType)
struct RTSPLUGIN_API FRTSOrderData
{
	GENERATED_BODY()

    FRTSOrderData();
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType);
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, AActor* InTarget);
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, FVector InLocation);
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, AActor* InTarget, FVector InLocation);
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, int32 InIndex);
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, int32 InIndex, AActor* InTarget);
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, int32 InIndex, FVector InLocation);
    FRTSOrderData(TSoftClassPtr<URTSOrder> InOrderType, int32 InIndex, AActor* InTarget, FVector InLocation);

    /**
	 * Get a textual representation of this order.
	 * @return A string describing the order.
	 */
    FString ToString() const;

    bool operator==(const FRTSOrderData& Other) const;

    bool operator!=(const FRTSOrderData& Other) const;

public:
	   /** Type of this order. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftClassPtr<URTSOrder> OrderType;

    /** Indicates that 'Location' is valid and should be used by this order. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (InlineEditConditionToggle = true))
    uint8 bUseLocation : 1;

    /**
     * Target location of the order. Use 'bUseLocation' to check if this location is really set and should be used.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bUseLocation))
    FVector Location;

    /** Target actor for this order. 'Null' if no order has been specified. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    AActor* Target;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Index = INDEX_NONE;
};

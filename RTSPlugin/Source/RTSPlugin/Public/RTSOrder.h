// Craig Duthie 2023

#pragma once

// Engine Includes
#include "CoreMinimal.h"

// Local Includes
#include "RTSOrderTargetData.h"
#include "RTSOrderResult.h"
#include "RTSOrderTargetType.h"
#include "RTSOrderProcessPolicy.h"
#include "RTSOrderErrorTags.h"
#include "RTSOrderTagRequirements.h"
#include "RTSOrderPreviewData.h"

// Generated Include
#include "RTSOrder.generated.h"

class UBehaviorTree;
class UTexture2D;
class URTSGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRTSOrderCallback, ERTSOrderResult, Result);

UCLASS(BlueprintType, Abstract, Blueprintable, Const)
class RTSPLUGIN_API URTSOrder : public UObject
{
	GENERATED_BODY()

public:
    URTSOrder();

    virtual void IssueOrder(AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index,
        FRTSOrderCallback Callback, const FVector& HomeLocation) const;

    /** Whether the specified actor can obey this kind of order. */
    virtual bool CanObeyOrder(const AActor* OrderedActor, int32 Index,
        FRTSOrderErrorTags* OutErrorTags = nullptr) const;

    /** Whether the specified actor and/or location is a valid target for this order. */
    virtual bool IsValidTarget(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index,
        FRTSOrderErrorTags* OutErrorTags = nullptr) const;

    /** Gets details about the preview for the order while choosing a target. */
    virtual FRTSOrderPreviewData GetOrderPreviewData(const AActor* OrderedActor, int32 Index) const;

    /**
	 * Gets a value that describes how an order is executed. This might determine how the order is displayed in the UI
	 * and it determines how the order is handled by the order system.
	 */
    virtual ERTSOrderProcessPolicy GetOrderProcessPolicy(const AActor* OrderedActor) const;

    /** Gets the target type this order is using. */
    virtual ERTSOrderTargetType GetTargetType(const AActor* OrderedActor, int32 Index) const;

    /** Whether this order type creates individual target locations for a group of actors */
    virtual bool IsCreatingIndividualTargetLocations(const AActor* OrderedActor, int32 Index) const;

    /** Whether this order allows auto orders when it is active. */
    virtual bool AreAutoOrdersAllowedDuringOrder() const;

    virtual void OrderCanceled(AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index) const;

    /**
	 * Gets the required range between the ordered actor and the target that is needed to fulfill the order.
	 * '0' value is returned if the order has no required range.
	 */
    virtual float GetRequiredRange(const AActor* OrderedActor, int32 Index) const;

    /** Gets the tag requirements for this order that must be full filled to be issued. */
    virtual void GetTagRequirements(const AActor* OrderedActor, int32 Index,
        FRTSOrderTagRequirements& OutTagRequirements) const;

    /** Gets the tag requirements for this order that must be full filled to be successful. */
    virtual void GetSuccessTagRequirements(const AActor* OrderedActor, int32 Index,
        FRTSOrderTagRequirements& OutTagRequirements) const;

    /** Gets the normal icon of the order. Can be shown in the UI. */
    virtual UTexture2D* GetNormalIcon(const AActor* OrderedActor, int32 Index) const;

    /** Gets the hovered icon of the order. Can be shown in the UI. */
    virtual UTexture2D* GetHoveredIcon(const AActor* OrderedActor, int32 Index) const;

    /** Gets the pressed icon of the order. Can be shown in the UI. */
    virtual UTexture2D* GetPressedIcon(const AActor* OrderedActor, int32 Index) const;

    /** Gets the disabled icon of the order. Can be shown in the UI. */
    virtual UTexture2D* GetDisabledIcon(const AActor* OrderedActor, int32 Index) const;

    /** Gets the Name of the order. Can be shown in the UI. */
    virtual FText GetName(const AActor* OrderedActor, int32 Index) const;

    /** Gets the Description of the order. Can be shown in the UI. */
    virtual FText GetDescription(const AActor* OrderedActor, int32 Index) const;

    /** Gets the index of the button when shown in the UI. */
    virtual int32 GetOrderButtonIndex() const;

    /** Checks whether to use a fixed index of the button when shown in the UI, instead of just lining it up among
     * others. */
    virtual bool HasFixedOrderButtonIndex() const;


protected:
     /* Describes how an order is executed. This might determine how the order is displayed in the UI and it determines
     * how the order is handled by the order system.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    ERTSOrderProcessPolicy OrderProcessPolicy;

    /** Tag requirements for an order that must be full filled to be issued. */
    UPROPERTY(Category = "Action Requirements", EditDefaultsOnly, BlueprintReadOnly)
    FRTSOrderTagRequirements TagRequirements;

    /** Tag requirements for an order that must be full filled to be to be successful. */
    UPROPERTY(Category = "Action Requirements", EditDefaultsOnly, BlueprintReadOnly)
    FRTSOrderTagRequirements SuccessTagRequirements;
};
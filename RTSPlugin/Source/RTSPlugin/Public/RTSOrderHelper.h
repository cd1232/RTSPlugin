#pragma once

// Engine Includes
#include "Kismet/BlueprintFunctionLibrary.h"

// Local Includes
#include "RTSOrderProcessPolicy.h"
#include "RTSOrderTargetData.h"
#include "RTSOrderErrorTags.h"
#include "RTSOrderTagRequirements.h"
#include "RTSOrderTargetType.h"
#include "RTSOrderData.h"

// Generated Include
#include "RTSOrderHelper.generated.h"

class UBehaviorTree;
class URTSOrderWithBehavior;
class URTSOrder;
class URTSGameplayAbility;

/**
 * Helper functions for accessing the default objects of order classes.
 */
UCLASS()
class RTSPLUGIN_API URTSOrderHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Whether the specified actor can obey this kind of order. */
    UFUNCTION(BlueprintPure)
    static bool CanObeyOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index);

    /** Whether the specified actor can obey this kind of order. */
    UFUNCTION(BlueprintPure, DisplayName = "Can Obey Order (With Error Tags)")
    static bool K2_CanObeyOrderWithErrorTags(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
        int32 Index, FRTSOrderErrorTags& OutErrorTags);

    /** Whether the specified actor can obey this kind of order. */
    static bool CanObeyOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index,
                             FRTSOrderErrorTags* OutErrorTags);

    /** Whether the specified actor and/or location is a valid target for this order. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static bool IsValidTarget(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                              const FRTSOrderTargetData& TargetData, int32 Index);

        /** Whether the specified actor and/or location is a valid target for this order. */
    UFUNCTION(Category = "RTS Order", BlueprintPure, DisplayName = "Is Valid Target (With Error Tags)")
    static bool K2_IsValidTargetWithErrorTags(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                              const FRTSOrderTargetData& TargetData, int32 Index,
        FRTSOrderErrorTags& OutErrorTags);

    /** Whether the specified actor and/or location is a valid target for this order. */
    static bool IsValidTarget(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                              const FRTSOrderTargetData& TargetData, int32 Index, FRTSOrderErrorTags* OutErrorTags);

    /** Issues the specified actor to obey this order on the specified target. */
    UFUNCTION(Category = "RTS Order", BlueprintCallable)
    static void IssueOrder(AActor* OrderedActor, const FRTSOrderData& Order);

    UFUNCTION(Category = "RTS Order", BlueprintCallable)
    static void CancelOrder(AActor* OrderedActor);

    /** Gets the tag requirements for this order that must be full filled to be issued. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static void GetOrderTagRequirements(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index,
                                        FRTSOrderTagRequirements& OutTagRequirements);

     /** Gets the target type the specified order is using. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static ERTSOrderTargetType GetTargetType(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
        int32 Index);

                                            /**
     * Gets the required range between the ordered actor and the target that is needed to fulfill the order.
     * '0' value is returned if the order has no required range.
     */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static float GetOrderRequiredRange(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
        int32 Index);

     /**
     * Whether the specified order can be considered as succeeded in regard to the specified ordered actor and order
     * target.
     */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static bool CanOrderBeConsideredAsSucceeded(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor,
                                                const FRTSOrderTargetData& TargetData, int32 Index = -1);


    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static ERTSOrderProcessPolicy GetOrderProcessPolicy(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor);

    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static UBehaviorTree* GetBehaviorTree(TSoftClassPtr<URTSOrderWithBehavior> OrderType);

    /** Whether to restart the behaviour tree whenever a new order of the specified type is issued. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static bool ShouldRestartBehaviourTree(TSoftClassPtr<URTSOrderWithBehavior> OrderType);

    /** Creates order target data using the specified target actor. */
    UFUNCTION(Category = "RTS Order", BlueprintPure)
    static FRTSOrderTargetData CreateOrderTargetData(const AActor* OrderedActor, AActor* TargetActor,
                                                     FVector TargetLocation);
};
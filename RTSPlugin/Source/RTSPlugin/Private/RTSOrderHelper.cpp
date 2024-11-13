// Craig Duthie 2023

// This Include
#include "RTSOrderHelper.h"

// Engine Includes
#include <AbilitySystemComponent.h>
#include <GameFramework/Actor.h>

// Local Includes
#include "RTSOrderWithBehavior.h"
#include "RTSOrder.h"
#include "RTSGlobalTags.h"
#include "RTSOrderComponent.h"
#include "RTSAbilitySystemHelper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RTSOrderHelper)

bool URTSOrderHelper::CanObeyOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index)
{
    return CanObeyOrder(OrderType, OrderedActor, Index, nullptr);
}

bool URTSOrderHelper::K2_CanObeyOrderWithErrorTags(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index, FRTSOrderErrorTags& OutErrorTags)
{
    // Clear the tag container. If this does not happen you may get old values when using this function from blueprint.
    OutErrorTags.MissingTags.Reset();
    OutErrorTags.BlockingTags.Reset();
    OutErrorTags.ErrorTags.Reset();

    return CanObeyOrder(OrderType, OrderedActor, Index, &OutErrorTags);
}

bool URTSOrderHelper::CanObeyOrder(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index, FRTSOrderErrorTags* OutErrorTags)
{
    if (OrderType == nullptr || !IsValid(OrderedActor))
    {
        return false;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }
    const URTSOrder* Order = OrderType->GetDefaultObject<URTSOrder>();
    const UAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<UAbilitySystemComponent>();
    if (AbilitySystem != nullptr)
    {
        FRTSOrderTagRequirements TagRequirements;
        Order->GetTagRequirements(OrderedActor, Index, TagRequirements);

        FGameplayTagContainer OrderedActorTags;
        AbilitySystem->GetOwnedGameplayTags(OrderedActorTags);

        if (OutErrorTags != nullptr)
        {
            if (!URTSAbilitySystemHelper::DoesSatisfyTagRequirementsWithResult(
                OrderedActorTags, TagRequirements.SourceRequiredTags, TagRequirements.SourceBlockedTags,
                OutErrorTags->MissingTags, OutErrorTags->BlockingTags))
            {
                return false;
            }
        }
        else
        {
            if (!URTSAbilitySystemHelper::DoesSatisfyTagRequirements(
                OrderedActorTags, TagRequirements.SourceRequiredTags, TagRequirements.SourceBlockedTags))
            {
                return false;
            }
        }
    }

    return Order->CanObeyOrder(OrderedActor, Index, OutErrorTags);
}

bool URTSOrderHelper::IsValidTarget(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index)
{
    return IsValidTarget(OrderType, OrderedActor, TargetData, Index, nullptr);
}

bool URTSOrderHelper::IsValidTarget(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index, FRTSOrderErrorTags* OutErrorTags)
{
    if (OrderType == nullptr)
    {
        return false;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    const URTSOrder* Order = OrderType->GetDefaultObject<URTSOrder>();

    ERTSOrderTargetType TargetType = Order->GetTargetType(OrderedActor, Index);
    if (TargetType == ERTSOrderTargetType::ACTOR)
    {
        if (!IsValid(TargetData.Actor))
        {
            if (OutErrorTags != nullptr)
            {
                OutErrorTags->ErrorTags.AddTag(URTSGlobalTags::AbilityActivationFailure_NoTarget());
            }
            return false;
        }

        FRTSOrderTagRequirements TagRequirements;
        Order->GetTagRequirements(OrderedActor, Index, TagRequirements);

        if (OutErrorTags != nullptr)
        {
            if (!URTSAbilitySystemHelper::DoesSatisfyTagRequirementsWithResult(
                TargetData.TargetTags, TagRequirements.TargetRequiredTags, TagRequirements.TargetBlockedTags,
                OutErrorTags->MissingTags, OutErrorTags->BlockingTags))
            {
                return false;
            }
        }

        else
        {
            if (!URTSAbilitySystemHelper::DoesSatisfyTagRequirements(
                TargetData.TargetTags, TagRequirements.TargetRequiredTags, TagRequirements.TargetBlockedTags))
            {
                return false;
            }
        }
    }

    return Order->IsValidTarget(OrderedActor, TargetData, Index, OutErrorTags);
}

void URTSOrderHelper::IssueOrder(AActor* OrderedActor, const FRTSOrderData& Order)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogTemp, Error, TEXT("URTSOrderHelper::IssueOrder: The specified actor is invalid."));
        return;
    }

    URTSOrderComponent* OrderComponent = OrderedActor->FindComponentByClass<URTSOrderComponent>();
    if (OrderComponent == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("URTSOrderHelper::IssueOrder: The specified actor has no order component."));
        return;
    }

    OrderComponent->IssueOrder(Order);
}

void URTSOrderHelper::CancelOrder(AActor* OrderedActor)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogTemp, Error, TEXT("URTSOrderHelper::IssueOrder: The specified actor is invalid."));
        return;
    }

    URTSOrderComponent* OrderComponent = OrderedActor->FindComponentByClass<URTSOrderComponent>();
    if (OrderComponent == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("URTSOrderHelper::IssueOrder: The specified actor has no order component."));
        return;
    }

    OrderComponent->CancelOrder();
}

bool URTSOrderHelper::K2_IsValidTargetWithErrorTags(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, const FRTSOrderTargetData& TargetData,
    int32 Index, FRTSOrderErrorTags& OutErrorTags)
{
    // Clear the tag container. If this does not happen you may get old values when using this function from blueprint.
    OutErrorTags.MissingTags.Reset();
    OutErrorTags.BlockingTags.Reset();
    OutErrorTags.ErrorTags.Reset();

    return IsValidTarget(OrderType, OrderedActor, TargetData, Index, &OutErrorTags);
}

void URTSOrderHelper::GetOrderTagRequirements(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index, FRTSOrderTagRequirements& OutTagRequirements)
{
    if (OrderType == nullptr)
    {
        UE_LOG(LogTemp, Error,
            TEXT("URTSOrderHelper::GetOrderTagRequirements: Error: Parameter 'OrderType' was 'nullptr'."));
        return;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetTagRequirements(OrderedActor, Index, OutTagRequirements);
}

ERTSOrderTargetType URTSOrderHelper::GetTargetType(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor /*= nullptr*/, int32 Index)
{
    if (OrderType == nullptr)
    {
        return ERTSOrderTargetType::NONE;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetTargetType(OrderedActor, Index);
}

float URTSOrderHelper::GetOrderRequiredRange(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, int32 Index)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogTemp, Error, TEXT("URTSOrderHelper::GetOrderRequiredRange: The specified actor is invalid."));
        return 0.0f;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetRequiredRange(OrderedActor, Index);
}

bool URTSOrderHelper::CanOrderBeConsideredAsSucceeded(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index /*= -1*/)
{
    if (OrderType == nullptr || !IsValid(OrderedActor))
    {
        return false;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    const URTSOrder* Order = OrderType->GetDefaultObject<URTSOrder>();
    const UAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<UAbilitySystemComponent>();

    check(AbilitySystem != nullptr);

    FRTSOrderTagRequirements TagRequirements;
    Order->GetSuccessTagRequirements(OrderedActor, Index, TagRequirements);

    FGameplayTagContainer OrderedActorTags;
    AbilitySystem->GetOwnedGameplayTags(OrderedActorTags);

    if (!URTSAbilitySystemHelper::DoesSatisfyTagRequirements(OrderedActorTags, TagRequirements.SourceRequiredTags,
        TagRequirements.SourceBlockedTags))
    {
        return false;
    }

    if (!URTSAbilitySystemHelper::DoesSatisfyTagRequirements(TargetData.TargetTags, TagRequirements.TargetRequiredTags,
        TagRequirements.TargetBlockedTags))
    {
        return false;
    }

    return true;
}

ERTSOrderProcessPolicy URTSOrderHelper::GetOrderProcessPolicy(TSoftClassPtr<URTSOrder> OrderType, const AActor* OrderedActor)
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogTemp, Error, TEXT("URTSOrderHelper::IsInstantOrder: The specified actor is invalid."));
        return ERTSOrderProcessPolicy::CAN_BE_CANCELED;
    }

    if (OrderType == nullptr)
    {
        return ERTSOrderProcessPolicy::CAN_BE_CANCELED;
    }

    // TODO Craig
    // Load Synchronous is bad
    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrder>()->GetOrderProcessPolicy(OrderedActor);
}

UBehaviorTree* URTSOrderHelper::GetBehaviorTree(TSoftClassPtr<URTSOrderWithBehavior> OrderType)
{
    if (OrderType == nullptr)
    {
        return nullptr;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrderWithBehavior>()->GetBehaviorTree();
}

bool URTSOrderHelper::ShouldRestartBehaviourTree(TSoftClassPtr<URTSOrderWithBehavior> OrderType)
{
    if (OrderType == nullptr)
    {
        return true;
    }

    if (!OrderType.IsValid())
    {
        OrderType.LoadSynchronous();
    }

    return OrderType->GetDefaultObject<URTSOrderWithBehavior>()->ShouldRestartBehaviourTree();
}

FRTSOrderTargetData URTSOrderHelper::CreateOrderTargetData(const AActor* OrderedActor, AActor* TargetActor, FVector TargetLocation)
{
    FRTSOrderTargetData TargetData;
    TargetData.Actor = TargetActor;
    TargetData.Location = TargetLocation;

    if (TargetActor == nullptr)
    {
        return TargetData;
    }

    FGameplayTagContainer SourceTags;
    FGameplayTagContainer TargetTags;
    URTSAbilitySystemHelper::GetSourceAndTargetTags(OrderedActor, TargetActor, SourceTags, TargetTags);

    TargetData.TargetTags = TargetTags;
    return TargetData;
}

// Craig Duthie 2023

// This Include
#include "RTSOrderWithBehavior.h"

// Local Includes
#include "RTSOrderAIController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RTSOrderWithBehavior)

URTSOrderWithBehavior::URTSOrderWithBehavior()
{
    bShouldRestartBehaviourTree = true;
    AcquisitionRadiusOverride = 0.0f;
    bIsAcquisitionRadiusOverridden = false;
}

void URTSOrderWithBehavior::IssueOrder(AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index, FRTSOrderCallback Callback, const FVector& HomeLocation) const
{
    if (!IsValid(OrderedActor))
    {
        UE_LOG(LogTemp, Error, TEXT("Ordered actor is invalid."));
        Callback.Broadcast(ERTSOrderResult::FAILED);
        return;
    }

    APawn* Pawn = Cast<APawn>(OrderedActor);
    if (Pawn == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("The specified actor '%s' is not a pawn."), *OrderedActor->GetName());
        Callback.Broadcast(ERTSOrderResult::FAILED);
        return;
    }

    // ACTION_ORDER
    // OrderHelper -> OrderComponent -> RTSOrderWithBehavior -> ARTSCharacterAIController
    // OrderHelper calls OrderComponent which then calls RTSOrderWithBehavior which then calls ARTSCharacterAIController
    ARTSOrderAIController* Controller = Cast<ARTSOrderAIController>(Pawn->GetController());
    if (Controller == nullptr)
    {
        UE_LOG(
            LogTemp, Error,
            TEXT(
                "The specified pawn '%s' does not have the required 'ARTSOrderAIController' to receive RTS orders."),
            *OrderedActor->GetName());
        Callback.Broadcast(ERTSOrderResult::FAILED);
        return;
    }

    AActor* TargetActor = TargetData.Actor;
    const FVector TargetLocation = TargetData.Location;

    FRTSOrderData Order(GetClass(), Index, TargetActor, TargetLocation);
    ERTSOrderTargetType TargetType = GetTargetType(OrderedActor, Index);

    Order.bUseLocation = TargetType == ERTSOrderTargetType::LOCATION || TargetType == ERTSOrderTargetType::DIRECTION;

    Controller->IssueOrder(Order, Callback, HomeLocation);
}

UBehaviorTree* URTSOrderWithBehavior::GetBehaviorTree() const
{
    return BehaviorTree;
}

bool URTSOrderWithBehavior::ShouldRestartBehaviourTree() const
{
    return bShouldRestartBehaviourTree;
}

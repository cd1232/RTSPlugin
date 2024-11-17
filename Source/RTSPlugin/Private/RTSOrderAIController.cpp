// Craig Duthie 2023

// This Include
#include "RTSOrderAIController.h"

// Engine Includes
#include <BehaviorTree/BlackboardComponent.h>

// Local Includes
#include "RTSOrderWithBehavior.h"
#include "RTSOrderHelper.h"
#include "RTSOrderBlackboardHelper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RTSOrderAIController)

void ARTSOrderAIController::IssueOrder(const FRTSOrderData& Order, FRTSOrderCallback Callback, const FVector& HomeLocation)
{
    UBehaviorTree* BehaviorTree = URTSOrderHelper::GetBehaviorTree(Order.OrderType.Get());

    if (BehaviorTree == nullptr)
    {
        Callback.Broadcast(ERTSOrderResult::FAILED);
        return;
    }

    CurrentOrderResultCallback = Callback;
    BehaviorTreeResult = EBTNodeResult::InProgress;

    SetBlackboardValues(Order, HomeLocation);

    // Stop any current orders and start over.
    ApplyOrder(Order, BehaviorTree);
}

void ARTSOrderAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // TODO Craig
  // Testing removing stop order
  // This will probably break blackboard component
//    StopOrder.LoadSynchronous();

    UBlackboardComponent* BlackboardComponent = nullptr;

    if (UseBlackboard(CharacterBlackboardAsset, BlackboardComponent))
    {
        //    // Setup blackboard.
        //    SetBlackboardValues(FRTSOrderData(StopOrder.Get()), InPawn->GetActorLocation());
    }

    //UBehaviorTree* BehaviorTree = URTSOrderHelper::GetBehaviorTree(StopOrder.Get());
    //RunBehaviorTree(BehaviorTree);
}

void ARTSOrderAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (Blackboard == nullptr)
    {
        return;
    }

    // BehaviorTreeEnded is responsible for changing the BehaviorTreeResult
    // It is a blueprint callable function

    switch (BehaviorTreeResult)
    {
    case EBTNodeResult::InProgress:
        break;
    case EBTNodeResult::Failed:
        CurrentOrderResultCallback.Broadcast(ERTSOrderResult::FAILED);
        break;
    case EBTNodeResult::Aborted:
        break;
    case EBTNodeResult::Succeeded:
        CurrentOrderResultCallback.Broadcast(ERTSOrderResult::SUCCEEDED);
        break;
    }

    BehaviorTreeResult = EBTNodeResult::InProgress;
}

TSoftClassPtr<URTSStopOrder> ARTSOrderAIController::GetStopOrder() const
{
    return StopOrder;
}

void ARTSOrderAIController::BehaviorTreeEnded(EBTNodeResult::Type Result)
{
    if (!VerifyBlackboard())
    {
        return;
    }

    switch (Result)
    {
    case EBTNodeResult::InProgress:
        return;
    case EBTNodeResult::Failed:
        BehaviorTreeResult = EBTNodeResult::Failed;
        return;
    case EBTNodeResult::Aborted:
        return;
    case EBTNodeResult::Succeeded:
        BehaviorTreeResult = EBTNodeResult::Succeeded;
        return;
    }
}

void ARTSOrderAIController::SetBlackboardValues(const FRTSOrderData& Order, const FVector& HomeLocation)
{
    if (!VerifyBlackboard())
    {
        UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerAIController::SetBlackboardValues failed - no blackboard"));
        return;
    }
    Blackboard->SetValueAsClass(URTSOrderBlackboardHelper::BLACKBOARD_KEY_ORDER_TYPE, Order.OrderType.Get());
    if (Order.bUseLocation)
    {
        // TODO Craig
        // NOTE(np): In A Year Of Rain, we're using a raycast to translate between 3D and 2D space.
        /*Blackboard->SetValueAsVector(URTSBlackboardHelper::BLACKBOARD_KEY_LOCATION,
                                     URTSUtilities::GetGroundLocation2D(this, Order.Location));*/
                                     // Blackboard->SetValueAsVector(URTSOrderBlackboardHelper::BLACKBOARD_KEY_LOCATION,
                                     //     FVector(Order.Location.X, Order.Location.Y, 0.0f));

        Blackboard->SetValueAsVector(URTSOrderBlackboardHelper::BLACKBOARD_KEY_LOCATION,
            Order.Location);
    }
    else
    {
        Blackboard->ClearValue(URTSOrderBlackboardHelper::BLACKBOARD_KEY_LOCATION);
    }

    Blackboard->SetValueAsObject(URTSOrderBlackboardHelper::BLACKBOARD_KEY_TARGET, Order.Target);
    Blackboard->SetValueAsInt(URTSOrderBlackboardHelper::BLACKBOARD_KEY_INDEX, Order.Index);
    Blackboard->SetValueAsFloat(URTSOrderBlackboardHelper::BLACKBOARD_KEY_RANGE,
        URTSOrderHelper::GetOrderRequiredRange(Order.OrderType, GetPawn(), Order.Index));

    Blackboard->SetValueAsVector(URTSOrderBlackboardHelper::BLACKBOARD_KEY_HOME_LOCATION, HomeLocation);
}

void ARTSOrderAIController::ApplyOrder(const FRTSOrderData& Order, UBehaviorTree* BehaviorTree)
{
    UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
    if (BehaviorTreeComponent == nullptr && BehaviorTree != nullptr)
    {
        //UBehaviorTree* BehaviorTree = URTSOrderHelper::GetBehaviorTree(StopOrder.Get());
        RunBehaviorTree(BehaviorTree);
    }

    BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
    if (BehaviorTreeComponent != nullptr && BehaviorTree != nullptr)
    {
        // Make sure to really restart the tree if the same same tree that is currently executing is passed in.
        UBehaviorTree* CurrentTree = BehaviorTreeComponent->GetRootTree();
        if (CurrentTree == BehaviorTree)
        {
            if (URTSOrderHelper::ShouldRestartBehaviourTree(Order.OrderType.Get()))
            {
                BehaviorTreeComponent->RestartTree();
            }
        }
        else
        {
            BehaviorTreeComponent->StartTree(*BehaviorTree, EBTExecutionMode::SingleRun);

            // TODO Craig
            // Is this the best way to stop a behavior tree/end an order?
           /* if (!BehaviorTreeComponent->OnBehaviorTreeFinished.IsBoundToObject(this))
            {
                BehaviorTreeComponent->OnBehaviorTreeFinished.AddUObject(this, &ARTSPlayerAIController::OnTreeFinished);
            }*/
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("BehaviorTreeComponent or BehaviorTree is null in ARTSPlayerAIController::ApplyOrder"));
    }
}

bool ARTSOrderAIController::VerifyBlackboard() const
{
    if (!Blackboard)
    {
        UE_LOG(
            LogTemp, Warning,
            TEXT("Blackboard not set up for %s, can't receive orders. Check AI Controller Class and Auto Possess AI."),
            *GetPawn()->GetName());
        return false;
    }

    return true;
}

void ARTSOrderAIController::OnTreeFinished()
{
    if (!VerifyBlackboard())
    {
        return;
    }

    // TODO
    // How do we know if it has succeeded or failed?
    BehaviorTreeResult = EBTNodeResult::Succeeded;

    UBehaviorTreeComponent* BehaviorTreeComponent = Cast<UBehaviorTreeComponent>(BrainComponent);
    /* if (BehaviorTreeComponent != nullptr)
     {
         BehaviorTreeComponent->OnBehaviorTreeFinished.RemoveAll(this);
     }*/
}

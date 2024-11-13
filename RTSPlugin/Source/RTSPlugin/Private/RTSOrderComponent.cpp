// Craig Duthie 2023

// This Include
#include "RTSOrderComponent.h"

// Engine Includes
#include <Net/UnrealNetwork.h>
#include <AbilitySystemBlueprintLibrary.h>
#include <AbilitySystemComponent.h>

// Local Includes
#include "RTSOrderAIController.h"
#include "RTSOrder.h"
#include "RTSStopOrder.h"
#include "RTSOrderErrorTags.h"
#include "RTSOrderTagRequirements.h"
#include "RTSOrderHelper.h"
#include "RTSGlobalTags.h"
#include "RTSAbilitySystemHelper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RTSOrderComponent)

// Sets default values for this component's properties
URTSOrderComponent::URTSOrderComponent()
{
    SetIsReplicatedByDefault(true);

    LastOrderHomeLocation = FVector::ZeroVector;
    bIsHomeLocationSet = false;
}

void URTSOrderComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(URTSOrderComponent, CurrentOrder);
    DOREPLIFETIME(URTSOrderComponent, OrderQueue);
}

void URTSOrderComponent::NotifyOnOrderChanged(const FRTSOrderData& NewOrder)
{
    UpdateOrderPreviews();

    OnOrderChanged.Broadcast(NewOrder);
}

// Called when the game starts
void URTSOrderComponent::BeginPlay()
{
    Super::BeginPlay();

    APawn* Pawn = Cast<APawn>(GetOwner());
    if (Pawn == nullptr)
    {
        return;
    }

    // TODO
    // NOTE(np): In A Year Of Rain, we're showing order previews for selected units.
   //// Register for selection events.
   //SelectableComponent = Pawn->FindComponentByClass<URTSSelectableComponent>();

   //if (SelectableComponent != nullptr)
   //{
   //    SelectableComponent->OnSelected.AddDynamic(this, &URTSOrderComponent::OnSelected);
   //    SelectableComponent->OnDeselected.AddDynamic(this, &URTSOrderComponent::OnDeselected);
   //}

   // Reset current order.
    CurrentOrder = FRTSOrderData();

	// Try to set the stop order if possible.
	ARTSOrderAIController* Controller = Cast<ARTSOrderAIController>(Pawn->GetController());
    if (Controller == nullptr)
    {
        return;
    }

    // TODO Craig
    // Testing removing stop order
    //StopOrder = Controller->GetStopOrder();

    //CurrentOrder = StopOrder;
    //IssueOrder(StopOrder);	
}

void URTSOrderComponent::StopBehaviorTree()
{
    APawn* pawn = Cast<APawn>(GetOwner());
    if (pawn)
    {
        AAIController* AIController = Cast<AAIController>(pawn->GetController());
        if (AIController)
        {
            if (UBehaviorTreeComponent* BehaviorTreeComp = AIController->FindComponentByClass<UBehaviorTreeComponent>())
            {
                BehaviorTreeComp->StopTree(EBTStopMode::Safe);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Failed to find ai controller in CancelOrder"));
        }
    }
}

bool URTSOrderComponent::IssueOrder(const FRTSOrderData& Order)
{
    AActor* Owner = GetOwner();

    // It is impossible for clients to issue orders. Clients need to issue orders using their player controller.
    if (!Owner->HasAuthority())
    {
        UE_LOG(LogTemp, Error,
            TEXT("The order %s was send from a client. It is impossible for clients to issue orders. Clients need "
                "to issue orders using their player controller. "),
            *Order.ToString());
        return false;
    }

    // Clear the order cue when another order is issued.
    OrderQueue.Empty();
    OnOrderQueueCleared.Broadcast();

    if (!Order.OrderType.IsValid())
    {
        Order.OrderType.LoadSynchronous();
    }

    // Do nothing if we are obeying exact the same order already (and I mean exact: Not only the same order type)
    if (CurrentOrder == Order)
    {
        return false;
    }

    bIsHomeLocationSet = false;

    // Abort current order.
    if (CurrentOrder.OrderType != StopOrder &&
        URTSOrderHelper::GetOrderProcessPolicy(Order.OrderType, Owner) != ERTSOrderProcessPolicy::INSTANT)
    {
        switch (URTSOrderHelper::GetOrderProcessPolicy(CurrentOrder.OrderType, Owner))
        {
        case ERTSOrderProcessPolicy::CAN_BE_CANCELED:
            OrderCanceled();

            if (CheckOrder(Order))
            {
                ObeyOrder(Order);
                return true;
            }
            // TODO Craig
            // Testing removing stop order
            /*else
            {

                ObeyStopOrder();
            }*/
            break;
        case ERTSOrderProcessPolicy::CAN_NOT_BE_CANCELED:
            // We cannot cancel our current order so we need to queue it up as next.
            OrderQueue.Add(Order);
            return true;
            break;
        case ERTSOrderProcessPolicy::INSTANT:
            // This should not be possible. Instant orders should not be set as current orders in the first place.
            check(0);
            break;
        default:
            check(0);
            break;
        }
    }
    else
    {
        if (CheckOrder(Order))
        {
            ObeyOrder(Order);
            return true;
        }
        // TODO Craig
        // Testing removing stop order
        //else
        //{
        //    ObeyStopOrder();
        //}
    }

    return false;
}

void URTSOrderComponent::CancelOrder()
{
    if (CurrentOrder != EmptyOrder)
    {
        OrderEnded(ERTSOrderResult::CANCELED);
        StopBehaviorTree();
    }
}

void URTSOrderComponent::ReceivedCurrentOrder()
{
    NotifyOnOrderChanged(CurrentOrder);
}

void URTSOrderComponent::ReceivedOrderQueue()
{
    UpdateOrderPreviews();
}

void URTSOrderComponent::UpdateOrderPreviews()
{
    // Clear old previews.
    for (AActor* OrderPreviewActor : OrderPreviewActors)
    {
        if (IsValid(OrderPreviewActor))
        {
            OrderPreviewActor->Destroy();
        }
    }

    OrderPreviewActors.Empty();

    // Don't spawn order previews for actors of others.
    AActor* Owner = GetOwner();

    if (!IsValid(Owner))
    {
        return;
    }

    // NOTE(np): In A Year Of Rain, we're only showing order previews for friendly players.
    /*URTSOwnerComponent* OwnerComponent = Owner->FindComponentByClass<URTSOwnerComponent>();

    if (!IsValid(OwnerComponent))
    {
        return;
    }

    if (!OwnerComponent->IsSameTeamAsController(UGameplayStatics::GetPlayerController(this, 0)))
    {
        return;
    }*/

    //Spawn new previews.
    bool bSelected = false; //SelectableComponent != nullptr && SelectableComponent->IsSelected();

    if ((bSelected && OrderQueue.Num() > 0))
    {
        CreateOrderPreviewActor(CurrentOrder);
    }

    for (const FRTSOrderData& OrderData : OrderQueue)
    {
        if (bSelected)
        {
            CreateOrderPreviewActor(OrderData);
        }
    }
}

void URTSOrderComponent::OrderCanceled()
{
    AActor* Owner = GetOwner();
    FRTSOrderTargetData TargetData =
        URTSOrderHelper::CreateOrderTargetData(Owner, CurrentOrder.Target, CurrentOrder.Location);

    UClass* OrderType = nullptr;

    if (!CurrentOrder.OrderType.IsValid())
    {
        OrderType = CurrentOrder.OrderType.LoadSynchronous();
    }
    else
    {
        OrderType = CurrentOrder.OrderType.Get();
    }

    UE_LOG(LogTemp, Log, TEXT("URTSOrderComponent::OrderCanceled"));

    if (OrderType != nullptr)
    {
        OrderType->GetDefaultObject<URTSOrder>()->OrderCanceled(Owner, TargetData, CurrentOrder.Index);
    }
}

void URTSOrderComponent::OrderEnded(ERTSOrderResult OrderResult)
{
    AActor* Owner = GetOwner();

    //UE_LOG(LogTemp, Log, TEXT("URTSOrderComponent::OrderEnded"));

    switch (OrderResult)
    {
    case ERTSOrderResult::FAILED:
    {
        CurrentOrder = EmptyOrder;
    }
    break;
    // TODO Craig
    // Testing removing stop order
    //if (StopOrder != nullptr)
    //{
    //    // OrderCanceled will be raised in IssueOrder.
    //    IssueOrder(FRTSOrderData(StopOrder));
    //}
    case ERTSOrderResult::CANCELED:
    {
        if (!URTSOrderHelper::CanOrderBeConsideredAsSucceeded(
            CurrentOrder.OrderType, Owner,
            URTSOrderHelper::CreateOrderTargetData(Owner, CurrentOrder.Target, CurrentOrder.Location),
            CurrentOrder.Index))
        {
            // OrderCanceled will be raised in IssueOrder.
              // TODO Craig
               // Testing removing stop order
            //IssueOrder(FRTSOrderData(StopOrder));
            CurrentOrder = EmptyOrder;
        }
    }
    break;
    // Fall through if succeeded        
    case ERTSOrderResult::SUCCEEDED:
    {
        if (OrderQueue.IsValidIndex(0))
        {
            const FRTSOrderData NewOrder = OrderQueue[0];

            if (CheckOrder(NewOrder))
            {
                OrderQueue.RemoveAt(0);
                ObeyOrder(NewOrder);
            }
            else
            {
                OrderQueue.Empty();
                // TODO Craig
                // Testing removing stop order
                //ObeyStopOrder();
                CurrentOrder = EmptyOrder;
            }
        }
        else
        {
            CurrentOrder = EmptyOrder;
        }
        // TODO Craig
        // Testing removing stop order
        //else if (StopOrder != nullptr)
        //{
        //    ObeyStopOrder();
        //}

    }
    break;
    default:
        break;
    }
}

bool URTSOrderComponent::CheckOrder(const FRTSOrderData& Order) const
{
    FRTSOrderErrorTags OrderErrorTags;

    AActor* OrderedActor = GetOwner();
    if (!Order.OrderType.IsValid())
    {
        Order.OrderType.LoadSynchronous();
    }

    TSubclassOf<URTSOrder> OrderType = Order.OrderType.Get();
    if (OrderType == nullptr)
    {
        UE_LOG(LogTemp, Error,
            TEXT("URTSOrderComponent::CheckOrder: The specified order for the actor '%s' is invalid."),
            *OrderedActor->GetName());
        return false;
    }

    if (!URTSOrderHelper::CanObeyOrder(OrderType.Get(), OrderedActor, Order.Index, &OrderErrorTags))
    {
        LogOrderErrorMessage(
            FString::Printf(TEXT("URTSOrderComponent::CheckOrder: The actor '%s' cannot obey the order '%s'."),
                *OrderedActor->GetName(), *OrderType->GetName()),
            OrderErrorTags);
        return false;
    }

    FRTSOrderTargetData TargetData = URTSOrderHelper::CreateOrderTargetData(OrderedActor, Order.Target, Order.Location);
    if (!URTSOrderHelper::IsValidTarget(OrderType.Get(), OrderedActor, TargetData, Order.Index, &OrderErrorTags))
    {
        LogOrderErrorMessage(
            FString::Printf(
                TEXT("URTSOrderComponent::CheckOrder: The actor '%s' was issued to obey the order '%s', but the "
                    "target data is invalid: %s"),
                *OrderedActor->GetName(), *OrderType->GetName(), *TargetData.ToString()),
            OrderErrorTags);
        return false;
    }

    return true;
}

void URTSOrderComponent::ObeyOrder(const FRTSOrderData& Order)
{
    AActor* Owner = GetOwner();
    FRTSOrderTargetData TargetData = URTSOrderHelper::CreateOrderTargetData(Owner, Order.Target, Order.Location);

    // Find the correct home location value for this order.
    FVector HomeLocation;
    if (bIsHomeLocationSet)
    {
        HomeLocation = LastOrderHomeLocation;
        bIsHomeLocationSet = false;
    }
    else
    {
        HomeLocation = Owner->GetActorLocation();
    }

    switch (URTSOrderHelper::GetOrderProcessPolicy(Order.OrderType, Owner))
    {
    case ERTSOrderProcessPolicy::INSTANT:
    {
        // An instant order is issued directly without changing the current order (ProductionOrder,
        // SetRallyPointOrder,
        // Some Abilities etc.).
        // Note: It is currently not possible to queue instant order because of the missing callback.
        // Maybe 'ObeyOrder' needs a return value that describes if the order is in progress or finished.
        Order.OrderType->GetDefaultObject<URTSOrder>()->IssueOrder(Owner, TargetData, Order.Index,
            FRTSOrderCallback(), HomeLocation);
    }
    break;
    case ERTSOrderProcessPolicy::CAN_BE_CANCELED:
    case ERTSOrderProcessPolicy::CAN_NOT_BE_CANCELED:
    {
        if (CurrentOrder.OrderType != StopOrder)
        {
            UnregisterTagListeners(CurrentOrder);
        }

        SetCurrentOrder(Order);

        FRTSOrderCallback Callback;
        Callback.AddDynamic(this, &URTSOrderComponent::OnOrderEndedCallback);

        if (!Order.OrderType.IsValid())
        {
            Order.OrderType.LoadSynchronous();
        }

        if (Order.OrderType != StopOrder)
        {
            RegisterTagListeners(Order);
        }

        Order.OrderType->GetDefaultObject<URTSOrder>()->IssueOrder(Owner, TargetData, Order.Index, Callback,
            HomeLocation);
    }
    break;
    default:
        check(0);
        break;
    }
}

void URTSOrderComponent::ObeyStopOrder()
{
    if (StopOrder == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s has no stop order set."), *GetOwner()->GetName());
        return;
    }

    if (!StopOrder.IsValid())
    {
        StopOrder.LoadSynchronous();
    }

    ObeyOrder(FRTSOrderData(StopOrder));
}

void URTSOrderComponent::RegisterTagListeners(const FRTSOrderData& Order)
{
    AActor* Owner = GetOwner();
    FRTSOrderTagRequirements TagRequirements;
    URTSOrderHelper::GetOrderTagRequirements(Order.OrderType, Owner, Order.Index, TagRequirements);

    UAbilitySystemComponent* OwnerAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);

    // Owner tags
    //

    if (OwnerAbilitySystem != nullptr)
    {
        FGameplayTagContainer OwnerTags;
        for (FGameplayTag Tag : TagRequirements.SourceRequiredTags)
        {
            // Don't register a delegate for permanent status tags.
            if (!Tag.MatchesTag(URTSGlobalTags::Status_Permanent()))
            {
                OwnerTags.AddTagFast(Tag);
            }
        }

        for (FGameplayTag Tag : TagRequirements.SourceBlockedTags)
        {
            // Don't register a delegate for permanent status tags.
            if (!Tag.MatchesTag(URTSGlobalTags::Status_Permanent()))
            {
                OwnerTags.AddTag(Tag);
            }
        }

        // TODO: Hard coded check for visibility change. Is their a more generic way todo this?
        if (TagRequirements.TargetRequiredTags.HasTag(URTSGlobalTags::Relationship_Visible()))
        {
            OwnerTags.AddTag(URTSGlobalTags::Status_Changing_Detector());
        }

        // Register a callback for each of the tags to check if it was added to or removed.
        for (FGameplayTag Tag : OwnerTags)
        {
            FOnGameplayEffectTagCountChanged& Delegate =
                OwnerAbilitySystem->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved);

            FDelegateHandle DelegateHandle = Delegate.AddUObject(this, &URTSOrderComponent::OnOwnerTagsChanged);
            RegisteredOwnerTagEventHandles.Add(Tag, DelegateHandle);
        }
    }

    // Target tags
    //

    ERTSOrderTargetType TargetType = URTSOrderHelper::GetTargetType(Order.OrderType, Owner, Order.Index);
    if (TargetType == ERTSOrderTargetType::ACTOR)
    {
        UAbilitySystemComponent* TargetAbilitySystem =
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Order.Target);

        if (TargetAbilitySystem != nullptr)
        {
            FGameplayTagContainer TargetTags;
            for (FGameplayTag Tag : TagRequirements.TargetRequiredTags)
            {
                // Don't register a delegate for permanent status tags.
                if (!Tag.MatchesTag(URTSGlobalTags::Status_Permanent()))
                {
                    TargetTags.AddTagFast(Tag);
                }
            }

            for (FGameplayTag Tag : TagRequirements.TargetBlockedTags)
            {
                // Don't register a delegate for permanent status tags.
                if (!Tag.MatchesTag(URTSGlobalTags::Status_Permanent()))
                {
                    TargetTags.AddTag(Tag);
                }
            }

            // TODO: Hard coded check for visibility change. Is their a more generic way todo this?
            if (TagRequirements.TargetRequiredTags.HasTag(URTSGlobalTags::Relationship_Visible()))
            {
                TargetTags.AddTag(URTSGlobalTags::Status_Changing_Stealthed());
            }

            // Register a callback for each of the tags to check if it was added to or removed.
            for (FGameplayTag Tag : TargetTags)
            {
                FOnGameplayEffectTagCountChanged& Delegate =
                    TargetAbilitySystem->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved);

                FDelegateHandle DelegateHandle = Delegate.AddUObject(this, &URTSOrderComponent::OnTargetTagsChanged);
                RegisteredTargetTagEventHandles.Add(Tag, DelegateHandle);
            }
        }
    }
}

void URTSOrderComponent::UnregisterTagListeners(const FRTSOrderData& Order)
{
    AActor* Owner = GetOwner();
    FRTSOrderTagRequirements TagRequirements;
    URTSOrderHelper::GetOrderTagRequirements(Order.OrderType, Owner, Order.Index, TagRequirements);

    UAbilitySystemComponent* OwnerAbilitySystem = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Owner);

    // Owner tags
    //

    if (OwnerAbilitySystem != nullptr)
    {
        for (TPair<FGameplayTag, FDelegateHandle> Pair : RegisteredOwnerTagEventHandles)
        {
            FOnGameplayEffectTagCountChanged& Delegate =
                OwnerAbilitySystem->RegisterGameplayTagEvent(Pair.Key, EGameplayTagEventType::NewOrRemoved);

            Delegate.Remove(Pair.Value);
        }

        RegisteredOwnerTagEventHandles.Empty();
    }

    // Target tags
    //

    ERTSOrderTargetType TargetType = URTSOrderHelper::GetTargetType(Order.OrderType, Owner, Order.Index);
    if (TargetType == ERTSOrderTargetType::ACTOR)
    {
        UAbilitySystemComponent* TargetAbilitySystem =
            UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Order.Target);

        if (TargetAbilitySystem != nullptr)
        {
            for (TPair<FGameplayTag, FDelegateHandle> Pair : RegisteredTargetTagEventHandles)
            {
                FOnGameplayEffectTagCountChanged& Delegate =
                    TargetAbilitySystem->RegisterGameplayTagEvent(Pair.Key, EGameplayTagEventType::NewOrRemoved);

                Delegate.Remove(Pair.Value);
            }

            RegisteredTargetTagEventHandles.Empty();
        }
    }
}

void URTSOrderComponent::LogOrderErrorMessage(const FString& Message, const FRTSOrderErrorTags& OrderErrorTags) const
{
    // TODO: Better formatting?
    FString FinalMessage = Message + TEXT(": ") + OrderErrorTags.ToString();
    UE_LOG(LogTemp, Warning, TEXT("%s"), *FinalMessage);
}

void URTSOrderComponent::SetCurrentOrder(FRTSOrderData NewOrder)
{
    LastOrder = CurrentOrder;

    CurrentOrder = NewOrder;

    NotifyOnOrderChanged(CurrentOrder);
}

void URTSOrderComponent::OnTargetTagsChanged(const FGameplayTag Tag, int32 NewCount)
{
    FRTSOrderTagRequirements TagRequirements;
    URTSOrderHelper::GetOrderTagRequirements(CurrentOrder.OrderType, GetOwner(), CurrentOrder.Index, TagRequirements);

    if ((NewCount && TagRequirements.TargetBlockedTags.HasTag(Tag)) ||
        (!NewCount && TagRequirements.TargetRequiredTags.HasTag(Tag)))
    {
        OrderEnded(ERTSOrderResult::CANCELED);
    }

    // TODO: Hard coded check for visibility change. Is their a more generic way todo this?
    else if (NewCount && Tag == URTSGlobalTags::Status_Changing_Stealthed() &&
        TagRequirements.TargetRequiredTags.HasTag(URTSGlobalTags::Relationship_Visible()))
    {
        if (!URTSAbilitySystemHelper::IsVisibleForActor(GetOwner(), CurrentOrder.Target))
        {
            OrderEnded(ERTSOrderResult::CANCELED);
        }
    }
}

void URTSOrderComponent::OnOwnerTagsChanged(const FGameplayTag Tag, int32 NewCount)
{
    FRTSOrderTagRequirements TagRequirements;
    URTSOrderHelper::GetOrderTagRequirements(CurrentOrder.OrderType, GetOwner(), CurrentOrder.Index, TagRequirements);

    if ((NewCount && TagRequirements.SourceBlockedTags.HasTag(Tag)) ||
        !NewCount && TagRequirements.SourceRequiredTags.HasTag(Tag))
    {
        OrderEnded(ERTSOrderResult::CANCELED);
    }

    // TODO: Hard coded check for visibility change. Is their a more generic way todo this?
    else if (!NewCount && Tag == URTSGlobalTags::Status_Changing_Detector() &&
        TagRequirements.TargetRequiredTags.HasTag(URTSGlobalTags::Relationship_Visible()))
    {
        if (!URTSAbilitySystemHelper::IsVisibleForActor(GetOwner(), CurrentOrder.Target))
        {
            OrderEnded(ERTSOrderResult::CANCELED);
        }
    }
}

void URTSOrderComponent::OnOrderEndedCallback(ERTSOrderResult OrderResult)
{
    StopBehaviorTree();

    OrderEnded(OrderResult);
}

AActor* URTSOrderComponent::CreateOrderPreviewActor(const FRTSOrderData& Order)
{
    if (OrderPreviewActorClass == nullptr)
    {
        return nullptr;
    }

    FActorSpawnParameters SpawnInfo;
    SpawnInfo.Instigator = Cast<APawn>(GetOwner());
    SpawnInfo.ObjectFlags |= RF_Transient;

    FVector Location;
    AActor* SpawnedActor = nullptr;

    // Spawn default order preview.
    ERTSOrderTargetType TargetType = URTSOrderHelper::GetTargetType(Order.OrderType, GetOwner(), Order.Index);
    switch (TargetType)
    {
    case ERTSOrderTargetType::NONE:
        break;
    case ERTSOrderTargetType::ACTOR:
        if (Order.Target != nullptr)
        {
            Location = Order.Target->GetActorLocation();
            SpawnedActor =
                GetWorld()->SpawnActor<AActor>(OrderPreviewActorClass, Location, FRotator::ZeroRotator, SpawnInfo);

            SpawnedActor->AttachToActor(Order.Target, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            OrderPreviewActors.Add(SpawnedActor);
        }
        break;
    case ERTSOrderTargetType::LOCATION:
    case ERTSOrderTargetType::DIRECTION:
        // NOTE(np): In A Year Of Rain, we're using a raycast to translate between 3D and 2D space.
        //Location = URTSUtilities::GetGroundLocation2D(this, Order.Location);
        Location = FVector(Order.Location.X, Order.Location.Y, 0.0f);
        SpawnedActor =
            GetWorld()->SpawnActor<AActor>(OrderPreviewActorClass, Location, FRotator::ZeroRotator, SpawnInfo);
        OrderPreviewActors.Add(SpawnedActor);
        break;
    case ERTSOrderTargetType::PASSIVE:
        break;
    default:
        break;
    }

    return SpawnedActor;
}


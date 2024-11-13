// Craig Duthie 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "RTSOrderData.h"
#include "RTSOrderErrorTags.h"

#include "RTSOrderComponent.generated.h"

class URTSOrder;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRTSOrderComponentOrderChangedSignature, const FRTSOrderData&, NewOrder);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRTSOrderComponentOrderQueueClearedSignature);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RTSPLUGIN_API URTSOrderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URTSOrderComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Event when the actor has received a new order. */
	virtual void NotifyOnOrderChanged(const FRTSOrderData& NewOrder);

	bool IssueOrder(const FRTSOrderData& Order);

	void CancelOrder();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void StopBehaviorTree();

private:
	UFUNCTION()
    void ReceivedCurrentOrder();

    UFUNCTION()
    void ReceivedOrderQueue();

	void UpdateOrderPreviews();

	void OrderCanceled();

	void OrderEnded(ERTSOrderResult OrderResult);


	bool CheckOrder(const FRTSOrderData& Order) const;
	void ObeyOrder(const FRTSOrderData& Order);
	void ObeyStopOrder();

	void RegisterTagListeners(const FRTSOrderData& Order);
	void UnregisterTagListeners(const FRTSOrderData& Order);

	void LogOrderErrorMessage(const FString& Message, const FRTSOrderErrorTags& OrderErrorTags) const;

	/** Store the current order of the actor. */
	void SetCurrentOrder(FRTSOrderData NewOrder);

	UFUNCTION()
    void OnTargetTagsChanged(const FGameplayTag Tag, int32 NewCount);

    UFUNCTION()
    void OnOwnerTagsChanged(const FGameplayTag Tag, int32 NewCount);

    UFUNCTION()
    void OnOrderEndedCallback(ERTSOrderResult OrderResult);

	AActor* CreateOrderPreviewActor(const FRTSOrderData& Order);

public:	
	/** Event when the actor has received a new order. */
	UPROPERTY(BlueprintAssignable)
	FRTSOrderComponentOrderChangedSignature OnOrderChanged;

	
	/** Event when the order queue has been cleared. */
    UPROPERTY(BlueprintAssignable)
	FRTSOrderComponentOrderQueueClearedSignature OnOrderQueueCleared;

private:
    UPROPERTY()
    TArray<AActor*> OrderPreviewActors;

    /** Class of the preview actor that is used to show the target location of an order. */
    UPROPERTY(Category = "RTS", BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
    TSubclassOf<AActor> OrderPreviewActorClass;

	UPROPERTY(BlueprintReadOnly, Category = "RTS", ReplicatedUsing = ReceivedCurrentOrder,
              meta = (AllowPrivateAccess = true))
    FRTSOrderData CurrentOrder;

    UPROPERTY(BlueprintReadOnly, Category = "RTS", ReplicatedUsing = ReceivedCurrentOrder,
              meta = (AllowPrivateAccess = true))
	FRTSOrderData LastOrder;

    UPROPERTY(BlueprintReadOnly, Category = "RTS", ReplicatedUsing = ReceivedOrderQueue,
              meta = (AllowPrivateAccess = true))
    TArray<FRTSOrderData> OrderQueue;

	UPROPERTY()
    TSoftClassPtr<URTSOrder> StopOrder;

	UPROPERTY()
	FRTSOrderData EmptyOrder;

	/**
	 * The handles of the delegates that are registered on the ability system of the actor owner to be able to abort
	 * the order if the requirements are nor longer fulfilled.
	 */
	TMap<FGameplayTag, FDelegateHandle> RegisteredOwnerTagEventHandles;

	/**
	 * The handles of the delegates that are registered on the ability system of the actor target to be able to abort
	 * the order if the requirements are nor longer fulfilled.
	 */
	TMap<FGameplayTag, FDelegateHandle> RegisteredTargetTagEventHandles;

	/** Last order home location if set. */
	FVector LastOrderHomeLocation;

	/** Indicates whether 'LastOrderHomeLocation' has a valid value. */
	bool bIsHomeLocationSet;

};

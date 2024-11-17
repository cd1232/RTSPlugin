// Craig Duthie 2023

#pragma once

// Engine Includes
#include <AIController.h>

// Local Includes
#include "RTSOrderData.h"
#include "RTSOrder.h"

// Generated Include
#include "RTSOrderAIController.generated.h"

// Forward Declares
class URTSStopOrder;

/**
 * 
 */
UCLASS()
class RTSPLUGIN_API ARTSOrderAIController : public AAIController
{
	GENERATED_BODY()

public:
	/** Issues this unit to obey the specified order. */
	void IssueOrder(const FRTSOrderData& Order, FRTSOrderCallback Callback, const FVector& HomeLocation);

	// AAIController
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	// AAIController

	UFUNCTION(BlueprintPure)
    TSoftClassPtr<URTSStopOrder> GetStopOrder() const;

	/** Called from the behavior tree to indicate that it has ended with the passed result. */
    UFUNCTION(BlueprintCallable)
    void BehaviorTreeEnded(EBTNodeResult::Type Result);

private:
	void SetBlackboardValues(const FRTSOrderData& Order, const FVector& HomeLocation);
	void ApplyOrder(const FRTSOrderData& Order, UBehaviorTree* BehaviorTree);

	bool VerifyBlackboard() const;

	UFUNCTION()
	void OnTreeFinished();

private:
	FRTSOrderCallback CurrentOrderResultCallback;

	/** Just used to cache the result of a behavior tree */
	EBTNodeResult::Type BehaviorTreeResult;

	/** The order to stop the character and put him in the idle state. */
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
    TSoftClassPtr<URTSStopOrder> StopOrder;

	/** Blackboard to use for holding all data relevant to the character AI. */
    UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UBlackboardData> CharacterBlackboardAsset;
};

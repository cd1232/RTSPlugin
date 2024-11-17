// Craig Duthie 2024

#pragma once

// Local Includes
#include "RTSOrderWithDisplay.h"
#include "RTSOrderTargetData.h"

// Generated Include
#include "RTSOrderWithBehavior.generated.h"

// Forward Declares
class UBehaviorTree;
class URTSGameplayAbility;

/**
 * 
 */
UCLASS()
class RTSPLUGIN_API URTSOrderWithBehavior : public URTSOrderWithDisplay
{
    GENERATED_BODY()

public:
    URTSOrderWithBehavior();

    virtual void IssueOrder(AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index,
        FRTSOrderCallback Callback, const FVector& HomeLocation) const override;

    UBehaviorTree* GetBehaviorTree() const;

    bool ShouldRestartBehaviourTree() const;

private:
       /** The behavior tree that should run in order to obey this order. */
    UPROPERTY( EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TObjectPtr<UBehaviorTree> BehaviorTree;

    /** Whether to restart the behavior tree whenever a new order of this type is issued. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    bool bShouldRestartBehaviourTree;
	
       /** The specific acquisition radius for this order. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
              meta = (AllowPrivateAccess = true, EditCondition = bIsAcquisitionRadiusOverridden))
    float AcquisitionRadiusOverride;

    /** Whether this order uses a specific acquisition radius. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
              meta = (AllowPrivateAccess = true, InlineEditConditionToggle = true))
    bool bIsAcquisitionRadiusOverridden;
};

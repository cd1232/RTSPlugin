// Craig Duthie 2023

#pragma once

// Local Includes
#include "RTSOrderWithBehavior.h"
#include "RTSOrderTargetType.h"

// Generated Include
#include "CharacterAIRTSOrder.generated.h"

// Forward Declares
class AActor;
class UBehaviorTree;
class URTSGameplayAbility;

/**
 * Represents an order type that is meant to be issued to pawn that are controlled by the 'URTSCharacterAIController'.
 */
UCLASS(BlueprintType, Abstract, Blueprintable)
class RTSPLUGIN_API UCharacterAIRTSOrder : public URTSOrderWithBehavior
{
    GENERATED_BODY()

public:
    UCharacterAIRTSOrder();

    //~ Begin URTSOrder InterfaceS
    virtual ERTSOrderTargetType GetTargetType(const AActor* OrderedActor, int32 Index) const override;
    virtual bool IsCreatingIndividualTargetLocations(const AActor* OrderedActor, int32 Index) const override;
    //virtual ERTSOrderGroupExecutionType GetGroupExecutionType(const AActor* OrderedActor, int32 Index) const override;
    //~ End URTSOrder Interface

protected:
    /** The target type this order is using. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    ERTSOrderTargetType TargetType;

    /**
     * To how many and which of the selected units should this order be issued to.
     */
    //UPROPERTY(Category = RTS, EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    //ERTSOrderGroupExecutionType GroupExecutionType;

    /** Whether this order type creates individual target locations for a group of actors */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bIsCreatingIndividualTargetLocations;
};

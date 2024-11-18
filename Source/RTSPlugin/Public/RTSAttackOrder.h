// Craig Duthie 2023

#pragma once

// Local Includes
#include "CharacterAIRTSOrder.h"
#include "RTSOrderTargetData.h"

// Generated Include
#include "RTSAttackOrder.generated.h"

class URTSGameplayAbility;

/**
 * 
 */
UCLASS(NotBlueprintType, Abstract, Blueprintable)
class RTSPLUGIN_API URTSAttackOrder : public UCharacterAIRTSOrder
{
	GENERATED_BODY()

public:
    URTSAttackOrder();

    //~ Begin URTSOrder Interface
    virtual void CreateIndividualTargetLocations(const TArray<AActor*>& OrderedActors,
        const FRTSOrderTargetData& TargetData,
        TArray<FVector2D>& OutTargetLocations) const override;

    virtual float GetRequiredRange(const AActor* OrderedActor, int32 Index) const override;

    virtual float GetTargetScore(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData,
        int32 Index) const override;
    virtual bool AreAutoOrdersAllowedDuringOrder() const override;
    //~ End URTSOrder Interface
	
};

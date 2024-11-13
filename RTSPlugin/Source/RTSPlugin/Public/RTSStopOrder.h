// Craig Duthie 2023

#pragma once

// Local Includes
#include "CharacterAIRTSOrder.h"

// Generated Include
#include "RTSStopOrder.generated.h"

/**
 * Stops the actor and switches it in the idle state.
 */
UCLASS(NotBlueprintType, Abstract, Blueprintable)
class RTSPLUGIN_API URTSStopOrder : public UCharacterAIRTSOrder
{
    GENERATED_BODY()

public:
    URTSStopOrder();

    //~ Begin URTSOrder Interface
    virtual bool AreAutoOrdersAllowedDuringOrder() const override;
    //~ End URTSOrder Interface
};


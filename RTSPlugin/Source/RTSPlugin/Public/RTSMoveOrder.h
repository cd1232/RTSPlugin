// Craig Duthie 2023

#pragma once

// Local Includes
#include "CharacterAIRTSOrder.h"

// Generated Include
#include "RTSMoveOrder.generated.h"

UCLASS(NotBlueprintType, Abstract, Blueprintable)
class RTSPLUGIN_API URTSMoveOrder : public UCharacterAIRTSOrder
{
    GENERATED_BODY()

public:
    URTSMoveOrder();
};
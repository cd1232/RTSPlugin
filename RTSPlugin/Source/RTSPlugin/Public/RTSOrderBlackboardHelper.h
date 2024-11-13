// Craig Duthie 2023

#pragma once

// Engine Includes
#include <CoreMinimal.h>
#include <Kismet/BlueprintFunctionLibrary.h>

// Generated Include
#include "RTSOrderBlackboardHelper.generated.h"

/**
 * 
 */
UCLASS()
class RTSPLUGIN_API URTSOrderBlackboardHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
    static const FName BLACKBOARD_KEY_ORDER_TYPE;
    static const FName BLACKBOARD_KEY_LOCATION;
    static const FName BLACKBOARD_KEY_TARGET;
    static const FName BLACKBOARD_KEY_INDEX;
    static const FName BLACKBOARD_KEY_RANGE;
    static const FName BLACKBOARD_KEY_HOME_LOCATION;
};

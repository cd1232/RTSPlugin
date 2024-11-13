// Craig Duthie 2023

// This Include
#include "RTSMoveOrder.h"

// Local Includes
#include "RTSOrderData.h"
#include "RTSGlobalTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RTSMoveOrder)

URTSMoveOrder::URTSMoveOrder()
{
	TargetType = ERTSOrderTargetType::LOCATION;

	TagRequirements.SourceBlockedTags.AddTag(URTSGlobalTags::Status_Changing_Immobilized());
	//TagRequirements.SourceBlockedTags.AddTag(URTSGlobalTags::Status_Changing_Constructing());
}

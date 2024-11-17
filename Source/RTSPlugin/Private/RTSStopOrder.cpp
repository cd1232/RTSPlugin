// Craig Duthie 2023

// This Include
#include "RTSStopOrder.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RTSStopOrder)

URTSStopOrder::URTSStopOrder()
{
	TargetType = ERTSOrderTargetType::NONE;
	bIsCreatingIndividualTargetLocations = false;
}

bool URTSStopOrder::AreAutoOrdersAllowedDuringOrder() const
{
	return true;
}

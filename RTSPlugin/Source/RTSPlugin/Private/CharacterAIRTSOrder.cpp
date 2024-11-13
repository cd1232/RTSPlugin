// Craig Duthie 2023

// This Include
#include "CharacterAIRTSOrder.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterAIRTSOrder)

UCharacterAIRTSOrder::UCharacterAIRTSOrder()
{
    TargetType = ERTSOrderTargetType::NONE;
}

ERTSOrderTargetType UCharacterAIRTSOrder::GetTargetType(const AActor* OrderedActor, int32 Index) const
{
    return TargetType;
}

bool UCharacterAIRTSOrder::IsCreatingIndividualTargetLocations(const AActor* OrderedActor, int32 Index) const
{
    return bIsCreatingIndividualTargetLocations;
}

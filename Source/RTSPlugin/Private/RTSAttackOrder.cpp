// Craig Duthie 2023

// This Include
#include "RTSAttackOrder.h"

// Local Includes
#include "RTSGlobalTags.h"
#include "RTSAbilitySystemHelper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RTSAttackOrder)

URTSAttackOrder::URTSAttackOrder()
{
    TargetType = ERTSOrderTargetType::ACTOR;
    bIsCreatingIndividualTargetLocations = true;

    TagRequirements.SourceRequiredTags.AddTag(URTSGlobalTags::Status_Permanent_CanAttack());
    TagRequirements.SourceBlockedTags.AddTag(URTSGlobalTags::Status_Changing_Unarmed());

    TagRequirements.TargetRequiredTags.AddTag(URTSGlobalTags::Status_Changing_IsAlive());
    TagRequirements.TargetRequiredTags.AddTag(URTSGlobalTags::Relationship_Visible());
    TagRequirements.TargetBlockedTags.AddTag(URTSGlobalTags::Status_Changing_Invulnerable());
    TagRequirements.TargetBlockedTags.AddTag(URTSGlobalTags::Relationship_Friendly());

    SuccessTagRequirements.TargetBlockedTags.AddTag(URTSGlobalTags::Status_Changing_IsAlive());
}

float URTSAttackOrder::GetRequiredRange(const AActor* OrderedActor, int32 Index) const
{
    return 10.0f;

    // TODO
    // Range would probably come from equipped weapon?
    //return URTSAbilitySystemHelper::GetAttributeValue(OrderedActor, URTSAttackAttributeSet::GetRangeAttribute());
}

//void URTSAttackOrder::CreateIndividualTargetLocations(const TArray<AActor*>& OrderedActors, const FRTSOrderTargetData& TargetData, TArray<FVector2D>& OutTargetLocations) const
//{
//    Super::CreateIndividualTargetLocations(OrderedActors, TargetData, OutTargetLocations);
//}
//
//
//float URTSAttackOrder::GetTargetScore(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index) const
//{
//    // TODO
//    return 0.0f;
//}

bool URTSAttackOrder::AreAutoOrdersAllowedDuringOrder() const
{
    return true;
}

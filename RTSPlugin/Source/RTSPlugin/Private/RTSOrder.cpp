// Craig Duthie 2023

// This Include
#include "RTSOrder.h"

// Local Includes
#include "RTSOrderProcessPolicy.h"
#include "RTSGlobalTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RTSOrder)

URTSOrder::URTSOrder()
{
	OrderProcessPolicy = ERTSOrderProcessPolicy::CAN_BE_CANCELED;
	TagRequirements.SourceRequiredTags.AddTag(URTSGlobalTags::Status_Changing_IsAlive());
}

void URTSOrder::IssueOrder(AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index, FRTSOrderCallback Callback, const FVector& HomeLocation) const
{
	check(0);
}

bool URTSOrder::CanObeyOrder(const AActor* OrderedActor, int32 Index, FRTSOrderErrorTags* OutErrorTags /*= nullptr*/) const
{
	return true;
}

bool URTSOrder::IsValidTarget(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index, FRTSOrderErrorTags* OutErrorTags /*= nullptr*/) const
{
	return true;
}

FRTSOrderPreviewData URTSOrder::GetOrderPreviewData(const AActor* OrderedActor, int32 Index) const
{
	return FRTSOrderPreviewData();
}

ERTSOrderProcessPolicy URTSOrder::GetOrderProcessPolicy(const AActor* OrderedActor) const
{
	return OrderProcessPolicy;
}

ERTSOrderTargetType URTSOrder::GetTargetType(const AActor* OrderedActor, int32 Index) const
{
	return ERTSOrderTargetType::NONE;
}

bool URTSOrder::IsCreatingIndividualTargetLocations(const AActor* OrderedActor, int32 Index) const
{
	return false;
}

bool URTSOrder::AreAutoOrdersAllowedDuringOrder() const
{
	return false;
}

void URTSOrder::OrderCanceled(AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index) const
{
	// empty
	UE_LOG(LogTemp, Log, TEXT("URTSOrder::OrderCanceled"));
}

float URTSOrder::GetRequiredRange(const AActor* OrderedActor, int32 Index) const
{
	return 0.0f;
}

void URTSOrder::GetTagRequirements(const AActor* OrderedActor, int32 Index, FRTSOrderTagRequirements& OutTagRequirements) const
{
	OutTagRequirements = TagRequirements;
}

void URTSOrder::GetSuccessTagRequirements(const AActor* OrderedActor, int32 Index, FRTSOrderTagRequirements& OutTagRequirements) const
{
	OutTagRequirements = SuccessTagRequirements;
}

UTexture2D* URTSOrder::GetNormalIcon(const AActor* OrderedActor, int32 Index) const
{
	return nullptr;
}

UTexture2D* URTSOrder::GetHoveredIcon(const AActor* OrderedActor, int32 Index) const
{
	return nullptr;
}

UTexture2D* URTSOrder::GetPressedIcon(const AActor* OrderedActor, int32 Index) const
{
	return nullptr;
}

UTexture2D* URTSOrder::GetDisabledIcon(const AActor* OrderedActor, int32 Index) const
{
	return nullptr;
}

FText URTSOrder::GetName(const AActor* OrderedActor, int32 Index) const
{
	return FText::FromString(GetClass()->GetName());
}

FText URTSOrder::GetDescription(const AActor* OrderedActor, int32 Index) const
{
	return FText();
}

int32 URTSOrder::GetOrderButtonIndex() const
{
	return -1;
}

bool URTSOrder::HasFixedOrderButtonIndex() const
{
	return false;
}

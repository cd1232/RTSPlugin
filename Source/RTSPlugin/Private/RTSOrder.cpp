// Craig Duthie 2023

// This Include
#include "RTSOrder.h"

// Local Includes
#include "RTSOrderProcessPolicy.h"
#include "RTSGlobalTags.h"
#include "RTSOrderGroupExecutionType.h"

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

void URTSOrder::CreateIndividualTargetLocations(const TArray<AActor*>& OrderedActors,
	const FRTSOrderTargetData& TargetData, TArray<FVector2D>& OutTargetLocations) const
{
}

bool URTSOrder::GetAcquisitionRadiusOverride(const AActor* OrderedActor, int32 Index, float& OutAcquisitionRadius) const
{
	return false;
}

TSoftClassPtr<URTSOrder> URTSOrder::GetFallbackOrder() const
{
	return FallbackOrder;
}

float URTSOrder::GetTargetScore(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index) const
{
	// TODO: Implement this function individually for each order type instead of here in the base class.
	if (!IsValid(OrderedActor))
	{
		return 0.0f;
	}

	float Distance = 0.0f;

	if (IsValid(TargetData.Actor))
	{
		// Subtract the half collision size of the target from the distance.
		// TODO: This is only relevant for melee units and should be ignored for ranged units.
		Distance = FVector::Dist2D(OrderedActor->GetActorLocation(), TargetData.Actor->GetActorLocation());

		// NOTE(np): In A Year Of Rain, unit collision radius is deducted from range to account for different unit sizes.
		//Distance -= URTSUtilities::GetActorCollisionSize(TargetData.Actor) / 2.0f;
	}

	else
	{
		Distance = FVector::Dist2D(OrderedActor->GetActorLocation(), FVector(TargetData.Location));
	}

	float AcquisitionRadius;
	if (!GetAcquisitionRadiusOverride(OrderedActor, Index, AcquisitionRadius))
	{
		// NOTE(np): In A Year Of Rain, units have a specific radius in which to automatically acquire targets.
		/*const URTSAttackComponent* AttackComponent = OrderedActor->FindComponentByClass<URTSAttackComponent>();
		if (AttackComponent == nullptr)
		{
			return 0.0f;
		}

		AcquisitionRadius = AttackComponent->GetAcquisitionRadius();*/
		AcquisitionRadius = 100000.0f;
	}

	return 1.0f - Distance / AcquisitionRadius;
}

ERTSOrderGroupExecutionType URTSOrder::GetGroupExecutionType(const AActor* OrderedActor, int32 Index) const
{
	return ERTSOrderGroupExecutionType::ALL;
}

bool URTSOrder::IsHumanPlayerAutoOrder(const AActor* OrderedActor, int32 Index) const
{
	return false;
}

bool URTSOrder::GetHumanPlayerAutoOrderInitialState(const AActor* OrderedActor, int32 Index) const
{
	return false;
}

bool URTSOrder::IsAIPlayerAutoOrder(const AActor* OrderedActor, int32 Index) const
{
	return false;
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

// Craig Duthie 2023

// This Include
#include "RTSUseAbilityOrder.h"

// Engine Includes
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/GameplayAbility.h"

// Local Includes
#include "RTSAbilitySystemComponent.h"
#include "RTSGameplayAbility.h"
#include "RTSOrderPreviewData.h"
#include "RTSOrderProcessPolicy.h"
#include "RTSGlobalTags.h"
#include "RTSAbilitySystemHelper.h"
#include "RTSOrderResult.h"

URTSUseAbilityOrder::URTSUseAbilityOrder()
{}

bool URTSUseAbilityOrder::CanObeyOrder(const AActor* OrderedActor, int32 Index, FRTSOrderErrorTags* OutErrorTags /*= nullptr*/) const
{
    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    const UGameplayAbility* Ability = GetAbility(AbilitySystem, Index);

    if (Ability != nullptr)
    {
        const TArray<FGameplayAbilitySpec>& AbilitySpecs = AbilitySystem->GetActivatableAbilities();
        for (const FGameplayAbilitySpec& Spec : AbilitySpecs)
        {
            if (Spec.Ability == Ability)
            {
                // Check if ability has been learned yet.
                if (Spec.Level <= 0)
                {
                    return false;
                }

                FGameplayTagContainer FailureTags;

                // Don't pass any source and target tags to can activate ability. These tags has already been checked in
                // 'URTSOrderHelper'. Only the activation required and activation blocked tags are checked here.
                if (!Ability->CanActivateAbility(Spec.Handle, AbilitySystem->AbilityActorInfo.Get(), nullptr, nullptr,
                    &FailureTags))
                {
                    if (OutErrorTags != nullptr)
                    {
                        OutErrorTags->ErrorTags = FailureTags;
                    }

                    return false;
                }

                // TODO
                // Not the nicest place to check this but it avoids adding this tag to every ability.
                //if (AbilitySystem->HasMatchingGameplayTag(URTSGlobalTags::Status_Changing_Constructing()))
                //{
                //    return false;
                //}

                return true;
            }
        }
    }

    return false;
}

ERTSOrderTargetType URTSUseAbilityOrder::GetTargetType(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return ERTSOrderTargetType::NONE;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));
    if (Ability != nullptr)
    {
        return Ability->GetTargetType();
    }

    return ERTSOrderTargetType::NONE;
}

void URTSUseAbilityOrder::IssueOrder(AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index,
    FRTSOrderCallback Callback, const FVector& HomeLocation) const
{
    if (OrderedActor == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Ordered actor is invalid."));
        Callback.Broadcast(ERTSOrderResult::FAILED);
        return;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    UGameplayAbility* Ability = GetAbility(AbilitySystem, Index);

    if (GetOrderProcessPolicy(OrderedActor) == ERTSOrderProcessPolicy::INSTANT)
    {
        FGameplayEventData EventData;
        URTSAbilitySystemHelper::CreateGameplayEventData(OrderedActor, TargetData, Ability->GetClass(), EventData);

        int32 TriggeredAbilities = URTSAbilitySystemHelper::SendGameplayEvent(OrderedActor, EventData);
        if (TriggeredAbilities > 0)
        {
            Callback.Broadcast(ERTSOrderResult::SUCCEEDED);
        }

        else
        {
            Callback.Broadcast(ERTSOrderResult::FAILED);
        }
    }
    else
    {
        Super::IssueOrder(OrderedActor, TargetData, Index, Callback, HomeLocation);
    }
}

UTexture2D* URTSUseAbilityOrder::GetNormalIcon(const AActor* OrderedActor, int32 Index) const
{
    return GetIcon(OrderedActor, Index);
}

UTexture2D* URTSUseAbilityOrder::GetHoveredIcon(const AActor* OrderedActor, int32 Index) const
{
    return GetIcon(OrderedActor, Index);
}

UTexture2D* URTSUseAbilityOrder::GetPressedIcon(const AActor* OrderedActor, int32 Index) const
{
    return GetIcon(OrderedActor, Index);
}

UTexture2D* URTSUseAbilityOrder::GetDisabledIcon(const AActor* OrderedActor, int32 Index) const
{
    return GetIcon(OrderedActor, Index);
}

FText URTSUseAbilityOrder::GetName(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return FText::FromString(TEXT("URTSUseAbilityOrder::GetName: Error: Parameter 'OrderedActor' was 'nullptr'."));
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));
    if (Ability == nullptr)
    {
        return FText::FromString(TEXT("URTSUseAbilityOrder::GetName: Error: Parameter 'Index' was invalid."));
    }

    return Ability->GetName();
}

FText URTSUseAbilityOrder::GetDescription(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return FText::FromString(
            TEXT("URTSUseAbilityOrder::GetDescription: Error: Parameter 'OrderedActor' was 'nullptr'."));
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));
    if (Ability == nullptr)
    {
        return FText::FromString(TEXT("URTSUseAbilityOrder::GetName: Error: Parameter 'Index' was invalid."));
    }

    return Ability->GetDescription(OrderedActor);
}

void URTSUseAbilityOrder::GetTagRequirements(const AActor* OrderedActor, int32 Index, FRTSOrderTagRequirements& OutTagRequirements) const
{
    if (OrderedActor == nullptr)
    {
        return;
    }

    URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability != nullptr)
    {
        return Ability->GetOrderTagRequirements(OutTagRequirements);
    }
}

float URTSUseAbilityOrder::GetRequiredRange(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return 0.0f;
    }

    URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    UGameplayAbility* Ability = GetAbility(AbilitySystem, Index);

    if (Ability != nullptr)
    {
        return AbilitySystem->GetAbilityRange(Ability->GetClass());
    }

    return 0.0f;
}

FRTSOrderPreviewData URTSUseAbilityOrder::GetOrderPreviewData(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return FRTSOrderPreviewData();
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));
    if (Ability == nullptr)
    {
        return FRTSOrderPreviewData();
    }

    return Ability->GetAbilityPreviewData();
}

ERTSOrderProcessPolicy URTSUseAbilityOrder::GetOrderProcessPolicy(const AActor* OrderedActor) const
{
    return ERTSOrderProcessPolicy::CAN_BE_CANCELED;
}

ERTSOrderGroupExecutionType URTSUseAbilityOrder::GetGroupExecutionType(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return ERTSOrderGroupExecutionType::ALL;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability != nullptr)
    {
        return Ability->GetGroupExecutionType();
    }

    return ERTSOrderGroupExecutionType::ALL;
}

bool URTSUseAbilityOrder::IsHumanPlayerAutoOrder(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return false;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability != nullptr)
    {
        return Ability->IsHumanPlayerAutoAbility();
    }

    return false;
}

bool URTSUseAbilityOrder::GetHumanPlayerAutoOrderInitialState(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return false;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability != nullptr)
    {
        return Ability->GetHumanPlayerAutoAutoAbilityInitialState();
    }

    return false;
}

bool URTSUseAbilityOrder::IsAIPlayerAutoOrder(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return false;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability != nullptr)
    {
        return Ability->IsAIPlayerAutoAbility();
    }

    return false;
}

bool URTSUseAbilityOrder::GetAcquisitionRadiusOverride(const AActor* OrderedActor, int32 Index,
    float& OutAcquisitionRadius) const
{
    if (OrderedActor == nullptr)
    {
        return false;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability != nullptr)
    {
        return Ability->GetAcquisitionRadiusOverride(OutAcquisitionRadius);
    }

    return false;
}

float URTSUseAbilityOrder::GetTargetScore(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData,
    int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return Super::GetTargetScore(OrderedActor, TargetData, Index);
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));

    if (Ability == nullptr || !Ability->IsTargetScoreOverriden())
    {
        Super::GetTargetScore(OrderedActor, TargetData, Index);
    }

    float TargetScore;
    Ability->GetTargetScore(OrderedActor, TargetData, Index, TargetScore);
    return TargetScore;
}

UGameplayAbility* URTSUseAbilityOrder::GetAbility(const URTSAbilitySystemComponent* AbilitySystem, int32 Index) const
{
    const TArray<TSubclassOf<UGameplayAbility>>& Abilities = AbilitySystem->GetInitialAndUnlockableAbilities();


    if (Abilities.IsValidIndex(Index) && Abilities[Index] != nullptr)
    {
        return Abilities[Index]->GetDefaultObject<UGameplayAbility>();
    }

    return nullptr;
}

UTexture2D* URTSUseAbilityOrder::GetIcon(const AActor* OrderedActor, int32 Index) const
{
    if (OrderedActor == nullptr)
    {
        return nullptr;
    }

    const URTSAbilitySystemComponent* AbilitySystem = OrderedActor->FindComponentByClass<URTSAbilitySystemComponent>();
    URTSGameplayAbility* Ability = Cast<URTSGameplayAbility>(GetAbility(AbilitySystem, Index));
    if (Ability == nullptr)
    {
        return nullptr;
    }

    return Ability->GetIcon();
}

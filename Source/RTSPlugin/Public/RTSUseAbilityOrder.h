// Craig Duthie 2023

#pragma once

// Engine Includes

// Local Includes
#include "RTSOrderGroupExecutionType.h"
#include "RTSOrderWithBehavior.h"
#include "RTSOrderTargetType.h"
#include "RTSOrderPreviewData.h"
#include "RTSOrderTargetData.h"

// Generated Include
#include "RTSUseAbilityOrder.generated.h"

// Forward Declares
class AActor;
struct FRTSOrderTargetData;
class URTSAbilitySystemComponent;
class UGameplayAbility;
class UTexture2D;
struct FRTSOrderErrorTags;
class URTSAbilitySystemComponent;
class URTSGameplayAbility;

/*
*   BT_UseAbility expects that the target data is already filled in
*   That means that if we had meteor from DA:Inquisition we would need to play start montage and hold that, spawn the target reticle/aoe and wait for user to confirm
*   only then would we actually issue the use ability order which would in turn call GA_UseAbility
* 
*   Does a selected AOE need to use Wait Target Data? No
*   Pass gameplay effect spec handle's to the spawned AOE object
*   and then end ability - let the spawned object handle applying the damage to things in it's range
*/

/**
 * Orders the actor to use a ability.
 */
UCLASS(NotBlueprintType, Abstract, Blueprintable, hideCategories = "RTS Display")
class RTSPLUGIN_API URTSUseAbilityOrder : public URTSOrderWithBehavior
{
    GENERATED_BODY()

public:
    URTSUseAbilityOrder();

    //~ Begin URTSOrder Interface
    virtual bool CanObeyOrder(const AActor* OrderedActor, int32 Index,
                              FRTSOrderErrorTags* OutErrorTags = nullptr) const override;
    virtual ERTSOrderTargetType GetTargetType(const AActor* OrderedActor, int32 Index) const override;
    virtual void IssueOrder(AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index,
                            FRTSOrderCallback Callback, const FVector& HomeLocation) const override;
    virtual UTexture2D* GetNormalIcon(const AActor* OrderedActor, int32 Index) const override;
    virtual UTexture2D* GetHoveredIcon(const AActor* OrderedActor, int32 Index) const override;
    virtual UTexture2D* GetPressedIcon(const AActor* OrderedActor, int32 Index) const override;
    virtual UTexture2D* GetDisabledIcon(const AActor* OrderedActor, int32 Index) const override;
    virtual FText GetName(const AActor* OrderedActor, int32 Index) const override;
    virtual FText GetDescription(const AActor* OrderedActor, int32 Index) const override;
    virtual void GetTagRequirements(const AActor* OrderedActor, int32 Index,
                                    FRTSOrderTagRequirements& OutTagRequirements) const override;

    virtual float GetRequiredRange(const AActor* OrderedActor, int32 Index) const override;
    virtual FRTSOrderPreviewData GetOrderPreviewData(const AActor* OrderedActor, int32 Index) const override;
    virtual ERTSOrderProcessPolicy GetOrderProcessPolicy(const AActor* OrderedActor) const override;
    virtual ERTSOrderGroupExecutionType GetGroupExecutionType(const AActor* OrderedActor, int32 Index) const override;
    virtual bool IsHumanPlayerAutoOrder(const AActor* OrderedActor, int32 Index) const override;
    virtual bool GetHumanPlayerAutoOrderInitialState(const AActor* OrderedActor, int32 Index) const override;
    virtual bool IsAIPlayerAutoOrder(const AActor* OrderedActor, int32 Index) const override;
    virtual bool GetAcquisitionRadiusOverride(const AActor* OrderedActor, int32 Index,
                                              float& OutAcquisitionRadius) const override;
    virtual float GetTargetScore(const AActor* OrderedActor, const FRTSOrderTargetData& TargetData, int32 Index) const;
    //~ End URTSOrder Interface

protected:
    virtual UGameplayAbility* GetAbility(const URTSAbilitySystemComponent* AbilitySystem, int32 Index) const;

private:
    UTexture2D* GetIcon(const AActor* OrderedActor, int32 Index) const;
};

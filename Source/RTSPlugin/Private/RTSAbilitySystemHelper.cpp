// Craig Duthie 2023

// This Include
#include "RTSAbilitySystemHelper.h"

// Engine Includes
#include <AbilitySystemComponent.h>
#include <AbilitySystemInterface.h>
#include <AbilitySystemBlueprintLibrary.h>

// Local Includes
#include "RTSGlobalTags.h"
#include "RTSAbilitySystemComponent.h"
#include "RTSGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RTSAbilitySystemHelper)

bool URTSAbilitySystemHelper::IsVisibleForActor(const AActor* Actor, const AActor* Other)
{
    if (Actor == nullptr || Other == nullptr)
    {
        return false;
    }

    // TODO
    // NOTE(np): In A Year Of Rain, the visible component stores for which players a units is currently visible.
    /*const URTSVisibleComponent* OtherVisibleComponent = Other->FindComponentByClass<URTSVisibleComponent>();
    if (OtherVisibleComponent != nullptr)
    {
        if (Other->HasAuthority())
        {
            return OtherVisibleComponent->IsVisibleForPlayer(Cast<AController>(Actor->GetOwner()));
        }

        return OtherVisibleComponent->IsVisibleForLocalClient();
    }

    else*/
    {
        // No visible component? It must be always visible!
        return true;
    }
}

bool URTSAbilitySystemHelper::DoesSatisfyTagRequirements(const FGameplayTagContainer& Tags, const FGameplayTagContainer& RequiredTags, const FGameplayTagContainer& BlockedTags)
{
    if (BlockedTags.Num() || RequiredTags.Num())
    {
        if (Tags.HasAny(BlockedTags))
        {
            return false;
        }

        if (!Tags.HasAll(RequiredTags))
        {
            return false;
        }
    }

    return true;
}

bool URTSAbilitySystemHelper::DoesSatisfyTagRequirementsWithResult(const FGameplayTagContainer& Tags, const FGameplayTagContainer& InRequiredTags, const FGameplayTagContainer& InBlockedTags,
    FGameplayTagContainer& OutMissingTags, FGameplayTagContainer& OutBlockingTags)
{
    bool bSuccess = true;
    for (FGameplayTag Tag : InBlockedTags)
    {
        if (Tags.HasTag(Tag))
        {
            bSuccess = false;
            OutBlockingTags.AddTag(Tag);
        }
    }

    for (FGameplayTag Tag : InRequiredTags)
    {
        if (!Tags.HasTag(Tag))
        {
            bSuccess = false;
            OutMissingTags.AddTag(Tag);
        }
    }

    return bSuccess;
}

void URTSAbilitySystemHelper::GetTags(const AActor* Actor, FGameplayTagContainer& OutGameplayTags)
{
    OutGameplayTags = OutGameplayTags.EmptyContainer;
    if (!IsValid(Actor))
    {
        return;
    }

    const UAbilitySystemComponent* AbilitySystem = Actor->FindComponentByClass<UAbilitySystemComponent>();
    if (AbilitySystem == nullptr)
    {
        return;
    }

    AbilitySystem->GetOwnedGameplayTags(OutGameplayTags);
}

FGameplayTagContainer URTSAbilitySystemHelper::GetRelationshipTags(const AActor* Actor, const AActor* Other)
{
    FGameplayTagContainer RelationshipTags;
    if (Actor == nullptr || Other == nullptr)
    {
        RelationshipTags.AddTag(URTSGlobalTags::Relationship_Neutral());
    }

    else if (Actor == Other)
    {
        RelationshipTags.AddTag(URTSGlobalTags::Relationship_Friendly());
        RelationshipTags.AddTag(URTSGlobalTags::Relationship_Self());
        RelationshipTags.AddTag(URTSGlobalTags::Relationship_Visible());
    }

    else
    {
        // TODO
        // NOTE(np): In A Year Of Rain, we're adding more relationship tags based on the current owners of both units.
        /* const URTSOwnerComponent* ActorOwnerComponent = Actor->FindComponentByClass<URTSOwnerComponent>();
         const URTSOwnerComponent* OtherOwnerComponent = Other->FindComponentByClass<URTSOwnerComponent>();

         if (ActorOwnerComponent == nullptr || OtherOwnerComponent == nullptr)
         {
             RelationshipTags.AddTag(URTSGlobalTags::Relationship_Neutral());
         }

         else
         {
             const ARTSPlayerState* ActorPlayerState = ActorOwnerComponent->GetPlayerOwner();
             const ARTSPlayerState* OtherPlayerState = OtherOwnerComponent->GetPlayerOwner();

             GetRelationshipTagsFromPlayers(ActorPlayerState, OtherPlayerState, RelationshipTags);
         }*/

        if (!RelationshipTags.HasTag(URTSGlobalTags::Relationship_Visible()) && IsVisibleForActor(Actor, Other))
        {
            RelationshipTags.AddTag(URTSGlobalTags::Relationship_Visible());
        }
    }

    return RelationshipTags;
}

int32 URTSAbilitySystemHelper::GetAbilityMaxLevel(UObject* WorldContextObject, TSubclassOf<UGameplayAbility> Ability)
{
    if (Ability == nullptr)
    {
        return 0;
    }

    URTSGameplayAbility* GameplayAbility = Cast<URTSGameplayAbility>(Ability->GetDefaultObject<UGameplayAbility>());

    if (GameplayAbility == nullptr)
    {
        return 0;
    }

    int32 MaxLevel = GameplayAbility->GetMaxLevel();

    if (MaxLevel > 0)
    {
        return MaxLevel;
    }

    return 0;   
}

void URTSAbilitySystemHelper::GetSourceAndTargetTags(const AActor* SourceActor, const AActor* TargetActor, FGameplayTagContainer& OutSourceTags, FGameplayTagContainer& OutTargetTags)
{
    GetTags(SourceActor, OutSourceTags);
    GetTags(TargetActor, OutTargetTags);

    FGameplayTagContainer RelationshipTags = URTSAbilitySystemHelper::GetRelationshipTags(SourceActor, TargetActor);

    OutSourceTags.AppendTags(RelationshipTags);
    OutTargetTags.AppendTags(RelationshipTags);
}

FName URTSAbilitySystemHelper::GetLastTagName(FGameplayTag Tag)
{
    if (!Tag.IsValid())
    {
        return FName();
    }

    TArray<FName> TagNames;
    UGameplayTagsManager::Get().SplitGameplayTagFName(Tag, TagNames);

    return TagNames.Last();
}

void URTSAbilitySystemHelper::CreateGameplayEventData(AActor* Source, const FRTSOrderTargetData& TargetData, TSubclassOf<UGameplayAbility> Ability, FGameplayEventData& OutEventData)
{
    if (Source == nullptr)
    {
        return;
    }

    URTSAbilitySystemComponent* AbilitySystem =
        Cast<URTSAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Source));

    FGameplayTagContainer InstigatorTags;
    URTSAbilitySystemHelper::GetTags(Source, InstigatorTags);

    URTSGameplayAbility* RTSAbility =
        Ability ? Cast<URTSGameplayAbility>(Ability->GetDefaultObject<UGameplayAbility>()) : nullptr;

    OutEventData.EventTag = RTSAbility ? RTSAbility->GetEventTriggerTag() : FGameplayTag();
    OutEventData.Instigator = Source;
    OutEventData.Target = TargetData.Actor;
    OutEventData.OptionalObject = nullptr;
    OutEventData.OptionalObject2 = nullptr;
    OutEventData.InstigatorTags = InstigatorTags;
    OutEventData.TargetTags = TargetData.TargetTags;
    OutEventData.EventMagnitude = AbilitySystem->GetLevel();
    OutEventData.TargetData = CreateAbilityTargetDataFromOrderTargetData(Source, TargetData, GetAbilityTargetType(Ability));

}

int32 URTSAbilitySystemHelper::SendGameplayEvent(AActor* Actor, FGameplayEventData Payload)
{
    if (IsValid(Actor))
    {
        IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(Actor);
        if (AbilitySystemInterface != nullptr)
        {
            UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
            if (AbilitySystemComponent != nullptr)
            {
                FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
                return AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
            }
        }
    }

    return 0;
}

ERTSOrderTargetType URTSAbilitySystemHelper::GetAbilityTargetType(TSubclassOf<UGameplayAbility> Ability)
{
    if (Ability == nullptr)
    {
        return ERTSOrderTargetType::NONE;
    }

    URTSGameplayAbility* GameplayAbility = Cast<URTSGameplayAbility>(Ability->GetDefaultObject<UGameplayAbility>());
    return GameplayAbility != nullptr ? GameplayAbility->GetTargetType() : ERTSOrderTargetType::NONE;

}

FGameplayAbilityTargetDataHandle URTSAbilitySystemHelper::CreateAbilityTargetDataFromOrderTargetData(AActor* OrderedActor, const FRTSOrderTargetData& OrderTargetData, ERTSOrderTargetType TargetType)
{
    switch (TargetType)
    {
    case ERTSOrderTargetType::ACTOR:
    {
        FGameplayAbilityTargetData_ActorArray* ActorData = new FGameplayAbilityTargetData_ActorArray();
        ActorData->TargetActorArray.Add(OrderTargetData.Actor);
        return FGameplayAbilityTargetDataHandle(ActorData);
    }
    case ERTSOrderTargetType::LOCATION:
    case ERTSOrderTargetType::DIRECTION:
    {
        FTransform Transform;

        FGameplayAbilityTargetingLocationInfo SourceLocation;
        SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
        Transform.SetLocation(OrderedActor->GetActorLocation());
        SourceLocation.LiteralTransform = Transform;

        FGameplayAbilityTargetingLocationInfo TargetLocation;
        TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
        Transform.SetLocation(FVector(OrderTargetData.Location.X, OrderTargetData.Location.Y, 0.0f));
        TargetLocation.LiteralTransform = Transform;

        FGameplayAbilityTargetData_LocationInfo* LocationData = new FGameplayAbilityTargetData_LocationInfo();
        LocationData->SourceLocation = SourceLocation;
        LocationData->TargetLocation = TargetLocation;

        return FGameplayAbilityTargetDataHandle(LocationData);
    }
    default:
        return nullptr;
    }
}

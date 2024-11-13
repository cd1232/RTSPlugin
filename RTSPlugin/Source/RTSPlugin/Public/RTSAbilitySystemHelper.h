#pragma once

// Engine Includes
#include <Kismet/BlueprintFunctionLibrary.h>
#include <GameplayTagContainer.h>
#include <Abilities/GameplayAbility.h>
#include <Abilities/GameplayAbilityTargetTypes.h>

// Local Includes
#include "RTSOrderTargetData.h"
#include "RTSOrderTargetType.h"

// Generated Include
#include "RTSAbilitySystemHelper.generated.h"

/** Helper function for the ability system. */
UCLASS(BlueprintType)
class RTSPLUGIN_API URTSAbilitySystemHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /** Whether 'Other' is visible for 'Actor'. */
    UFUNCTION(Category = "Ability|Tags", BlueprintPure)
    static bool IsVisibleForActor(const AActor* Actor, const AActor* Other);

    /** Checks if the specified tags has all of the specified required tags and none of the specified blocked tags. */
    UFUNCTION(Category = "Ability|Tags", BlueprintPure)
    static bool DoesSatisfyTagRequirements(const FGameplayTagContainer& Tags, const FGameplayTagContainer& RequiredTags,
                                           const FGameplayTagContainer& BlockedTags);

    /** Checks if the specified tags has all of the specified required tags and none of the specified blocked tags. */
    UFUNCTION(Category = "Ability|Tags", BlueprintPure)
    static bool DoesSatisfyTagRequirementsWithResult(const FGameplayTagContainer& Tags,
                                                     const FGameplayTagContainer& InRequiredTags,
                                                     const FGameplayTagContainer& InBlockedTags,
                                                     FGameplayTagContainer& OutMissingTags,
                                                     FGameplayTagContainer& OutBlockingTags);

    /** Gets the gameplay tags of the specified actor. */
    UFUNCTION(Category = "Ability|Tags", BlueprintPure)
    static void GetTags(const AActor* Actor, FGameplayTagContainer& OutGameplayTags);

        /**
     * Gets the tags describing the relationship of the first actor to the other (friendly, hostile, neutral, same
     * player, visibility.
     */
    UFUNCTION(Category = "Ability|Tags", BlueprintPure)
    static FGameplayTagContainer GetRelationshipTags(const AActor* Actor, const AActor* Other);

    // NOTE(np): In A Year Of Rain, we're adding relationship tags based on the team assignments of both players.
    ///**
    // * Gets the tags describing the relationship of the first player to the other (friendly, hostile, neutral, same
    // * player, visibility).
    // */
    //UFUNCTION(Category = "RTS Ability|Tags", BlueprintPure)
    //static void GetRelationshipTagsFromPlayers(const ARTSPlayerState* ActorPlayerState,
    //                                           const ARTSPlayerState* OtherPlayerState,
    //                                           FGameplayTagContainer& OutRelationshipTags);

    /**
     * Gets the gameplay tags of the specified actors, including relationship tags.
     */
    UFUNCTION(Category = "Ability|Tags", BlueprintPure)
    static void GetSourceAndTargetTags(const AActor* SourceActor, const AActor* TargetActor,
                                       FGameplayTagContainer& OutSourceTags, FGameplayTagContainer& OutTargetTags);


    /** Fills a 'FGameplayEventData' with values from the specified ability and order target data. */
    UFUNCTION(Category = "Ability|Event", BlueprintPure)
    static void CreateGameplayEventData(AActor* Source, const FRTSOrderTargetData& TargetData,
                                        TSubclassOf<UGameplayAbility> Ability, FGameplayEventData& OutEventData);

     /**
     * Sends a gameplay event to the specified actor. Returns the number of successful ability activations triggered by
     * the event.
     */
    UFUNCTION(Category = "Ability|Event", BlueprintCallable)
    static int32 SendGameplayEvent(AActor* Actor, FGameplayEventData Payload);

                                            /** Gets the target type of the specified gameplay ability class. */
    UFUNCTION(Category = "Ability|Abilities", BlueprintPure)
    static ERTSOrderTargetType GetAbilityTargetType(TSubclassOf<UGameplayAbility> Ability);

    /** Creates an ability target data from the specified order target data. */
    static FGameplayAbilityTargetDataHandle
        CreateAbilityTargetDataFromOrderTargetData(AActor* OrderedActor, const FRTSOrderTargetData& OrderTargetData,
            ERTSOrderTargetType TargetType);

};

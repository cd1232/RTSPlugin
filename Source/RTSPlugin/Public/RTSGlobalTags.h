#pragma once

// Engine Includes
#include <Kismet/BlueprintFunctionLibrary.h>
#include <GameplayTagContainer.h>
#include <NativeGameplayTags.h>

// Generated Include
#include "RTSGlobalTags.generated.h"

namespace RTSAbilityTags
{
    UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Ability_MageBolt);
}

/** Global tags that need to be exposed to C++. */
UCLASS()
class RTSPLUGIN_API URTSGlobalTags : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // ---------------------------------------------------------------------------------------------------
    // Permanent status tags
    // ---------------------------------------------------------------------------------------------------
    static const FGameplayTag& Status_Permanent();

    /** Whether the actor is generally able to attack. This tag should never be removed. */
    static const FGameplayTag& Status_Permanent_CanAttack();

    // ---------------------------------------------------------------------------------------------------
    // Changing status tags
    // ---------------------------------------------------------------------------------------------------

    
    /** Whether the actor is still alive. This is only changed when the actor dies. */
    static const FGameplayTag& Status_Changing_IsAlive();

    /** Whether the actor is currently not able to attack. This might change during gameplay. */
    static const FGameplayTag& Status_Changing_Unarmed();

    /** Whether the actor is currently able to detect stealthed units. This might change during gameplay. */
    static const FGameplayTag& Status_Changing_Detector();

    /** Whether the actor is currently hidden from most enemies. This might change during gameplay. */
    static const FGameplayTag& Status_Changing_Stealthed();

    /** Whether the actor is currently not able to move. This might change during gameplay. */
    static const FGameplayTag& Status_Changing_Immobilized();

    /** Whether the actor is currently invulnerable. */
    static const FGameplayTag& Status_Changing_Invulnerable();


    // ---------------------------------------------------------------------------------------------------
    // Ability activation failure tags
    // --------------------------------------------------------------------------------------------------

    /** The ability cannot be activated because no target has been specified. */
    static const FGameplayTag& AbilityActivationFailure_NoTarget();

    // ---------------------------------------------------------------------------------------------------
    // Relationship tags
    // ---------------------------------------------------------------------------------------------------

    /** Relationship parent tag. */
    static const FGameplayTag& Relationship();

    /** Whether the two actor references resolve to the same actor. */
    static const FGameplayTag& Relationship_Self();

    /** Whether the relationship to this actor is friendly. */
    static const FGameplayTag& Relationship_Friendly();

    /** Whether the relationship to this actor is hostile. */
    static const FGameplayTag& Relationship_Hostile();

    /** Whether the relationship to this actor is neutral. */
    static const FGameplayTag& Relationship_Neutral();

    /** Whether this actor is controlled by the same player. */
    static const FGameplayTag& Relationship_SamePlayer();

    /** Whether the actor is visible. */
    static const FGameplayTag& Relationship_Visible();
};

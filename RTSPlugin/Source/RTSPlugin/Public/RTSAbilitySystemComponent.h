// Craig Duthie 2023

#pragma once

// Engine Includes
#include <AttributeSet.h>
#include <AbilitySystemComponent.h>

// Local Includes
#include "RTSGameplayAbility.h"

// Generated Include
#include "RTSAbilitySystemComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, URTSAbilitySystemComponent*, SourceASC, float, UnmitigatedDamage, float, MitigatedDamage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRTSAbilitySystemComponentAbilityEndedSignature,
	TSubclassOf<UGameplayAbility>, Ability, FGameplayAbilitySpecHandle,
	AbilitySpecHandle, bool, bWasCancelled);

/**
 * 
 */
UCLASS()
class RTSPLUGIN_API URTSAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void ReceiveDamage(URTSAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage);

	/**
	 * Gets the minimum range between the caster and the target that is needed to activate the specified ability.
	 * '0' value is returned if the ability has no range.
	 */
	float GetAbilityRange(TSubclassOf<URTSGameplayAbility> Ability);

    /** Gets the combined list of all initial and unlockable abilities. */
    //UFUNCTION(Category = RTS, BlueprintPure)
   // TArray<TSubclassOf<UGameplayAbility>> GetInitialAndUnlockableAbilities() const;

	/** Adds a tag to this ability system. */
	void AddTag(const FGameplayTag Tag);

	/** Removes a tag from this ability system. */
	void RemoveTag(const FGameplayTag Tag);

	/** Adds the specified tags to this ability system. */
	void AddTags(const FGameplayTagContainer& Tags);

	/** Removes the tags from this ability system. */
	void RemoveTags(const FGameplayTagContainer& Tags);

	/** Gets the current level of this actor. */
    UFUNCTION(Category = RTS, BlueprintCallable)
    int32 GetLevel() const;

    /** Gets the combined list of all initial and unlockable abilities. */
    UFUNCTION(Category = RTS, BlueprintPure)
    TArray<TSubclassOf<UGameplayAbility>> GetInitialAndUnlockableAbilities() const;

	int32 GetAbilityIndex(TSubclassOf<UGameplayAbility> Ability) const;

	bool TryActivateAbilityByClassWithEventData(TSubclassOf<UGameplayAbility> Ability, const FGameplayEventData& EventData, bool bAllowRemoteActivation = false);
	bool TryActivateAbilityWithEventData(FGameplayAbilitySpecHandle AbilityToActivate, const FGameplayEventData& EventData, bool bAllowRemoteActivation = false);

protected:
	void AbilityEndedCallback(const FAbilityEndedData& AbilityEndedData);

public:
	bool bHaveAbilitiesBeenGiven = false;
	bool bHaveStartupEffectsBeenApplied = false;

	UPROPERTY(BlueprintAssignable)
	FReceivedDamageDelegate ReceivedDamageDelegate;

	/** Event that is invoked when an ability of this ability system has ended. */
    UPROPERTY(BlueprintAssignable, Category = "RTS")
	FRTSAbilitySystemComponentAbilityEndedSignature OnGameplayAbilityEnded;

protected:
    /** If changes of the owning tags should be logged. */
    UPROPERTY(Category = "RTS", EditDefaultsOnly)
    bool bLogTagChanges = false;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "RTS")
	TArray<TSubclassOf<URTSGameplayAbility>> InitialAbilities;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "RTS")
	TArray<TSubclassOf<URTSGameplayAbility>> UnlockableAbilities;


private:

    /** Current level of the actor. */
    UPROPERTY(Category = "RTS|Level", BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true, ClampMin = 1, UIMin = 1))
    int32 Level;
};

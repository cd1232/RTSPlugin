// Craig Duthie 2023

#pragma once

// Engine Includes
#include <AttributeSet.h>
#include <AbilitySystemComponent.h>

// Local Includes
#include "RTSAbilitySystemHelper.h"
#include "RTSAutoOrderProvider.h"
#include "RTSGameplayAbility.h"

// Generated Include
#include "RTSAbilitySystemComponent.generated.h"

class URTSUseAbilityOrder;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReceivedDamageDelegate, URTSAbilitySystemComponent*, SourceASC, float,
                                               UnmitigatedDamage, float, MitigatedDamage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRTSAbilitySystemComponentAbilityEndedSignature,
	TSubclassOf<UGameplayAbility>, Ability, FGameplayAbilitySpecHandle,
	AbilitySpecHandle, bool, bWasCancelled);

/**
 * 
 */
UCLASS()
class RTSPLUGIN_API URTSAbilitySystemComponent : public UAbilitySystemComponent, public IRTSAutoOrderProvider
{
	GENERATED_BODY()

public:
	URTSAbilitySystemComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;

	virtual void ReceiveDamage(URTSAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage);

	/**
	 * Gets the minimum range between the caster and the target that is needed to activate the specified ability.
	 * '0' value is returned if the ability has no range.
	 */
	float GetAbilityRange(TSubclassOf<URTSGameplayAbility> Ability);

	//~ Begin IRTSAutoOrderProvider Interface
	void GetAutoOrders_Implementation(TArray<FRTSOrderTypeWithIndex>& OutAutoOrders);
	//~ End IRTSAutoOrderProvider Interface

	/** Adds a tag to this ability system. */
	void AddTag(const FGameplayTag Tag);

	/** Removes a tag from this ability system. */
	void RemoveTag(const FGameplayTag Tag);

	/** Adds the specified tags to this ability system. */
	void AddTags(const FGameplayTagContainer& Tags);

	/** Removes the tags from this ability system. */
	void RemoveTags(const FGameplayTagContainer& Tags);

    /** Gets the name of the owner of this component. */
    UFUNCTION(Category = RTS, BlueprintPure)
    FName GetName() const;

    /** Gets the tag that is associated with the owner of this component. */
    FGameplayTag GetNameTag() const;

    /** Sets the level of this actor. This might change the attributes of the actor. */
    UFUNCTION(Category = RTS, BlueprintCallable)
    void SetLevel(int32 NewLevel);

    /** Gets the current level of this actor. */
    UFUNCTION(Category = RTS, BlueprintCallable)
    int32 GetLevel() const;

    /** Gets the XP this actor grants to its killer. The curve table value depends on the level of this actor.  */
    UFUNCTION(Category = RTS, BlueprintPure)
    float GetGrantedXP() const;

    /** Gets the total amount of XP required for reaching the specified level. */
    UFUNCTION(Category = RTS, BlueprintPure)
    float GetTotalXPRequiredForLevel(int32 InLevel) const;

    /** Gets the amount of XP gained since the last level-up. */
    UFUNCTION(Category = RTS, BlueprintPure)
    float GetCurrentLevelXP() const;

    /** Gets the total amount of XP required for the next level-up. */
    UFUNCTION(Category = RTS, BlueprintPure)
    float GetNextLevelXP() const;

    /** Gets the progress of the unit towards reaching the next level [0..1]. */
    UFUNCTION(Category = RTS, BlueprintPure)
    float GetCurrentLevelXPProgress() const;

    /** Gets the lifetime collected XP of the actor. */
    UFUNCTION(Category = RTS, BlueprintPure)
    float GetCollectedXP() const;

    /** Adds the specified amount of XP to the actor. */
    UFUNCTION(Category = RTS, BlueprintCallable)
    void AddCollectedXP(float AdditionalCollectedXP);

    /** Whether this actor can level up.  */
    UFUNCTION(Category = RTS, BlueprintPure)
    bool CanLevelUp() const;

    /** Gets the current ability points (granted on level up and required to increase ability levels). */
    UFUNCTION(Category = RTS, BlueprintPure)
    int32 GetAbilityPoints() const;

    /** Sets the current ability points (granted on level up and required to increase ability levels). */
    void SetAbilityPoints(int32 NewAbilityPoints);

    /** Spends an ability point to increase the level of the specified ability. */
    UFUNCTION(Category = RTS, BlueprintCallable)
    void IncreaseAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass, bool bUseAbilityPoint = true);

    /** Gets the combined list of all initial and unlockable abilities. */
    UFUNCTION(Category = RTS, BlueprintPure)
    TArray<TSubclassOf<UGameplayAbility>> GetInitialAndUnlockableAbilities() const;

	int32 GetAbilityIndex(TSubclassOf<UGameplayAbility> Ability) const;

	bool TryActivateAbilityByClassWithEventData(TSubclassOf<UGameplayAbility> Ability, const FGameplayEventData& EventData, bool bAllowRemoteActivation = false);
	bool TryActivateAbilityWithEventData(FGameplayAbilitySpecHandle AbilityToActivate, const FGameplayEventData& EventData, bool bAllowRemoteActivation = false);

protected:
	void InitializeAttributes(int AttributeLevel, bool bInitialInit);
	
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

	/** Order type that is used to issue a unit to activate an ability. */
	UPROPERTY(Category = "RTS|Orders", BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	TSoftClassPtr<URTSUseAbilityOrder> UseAbilityOrder;
	
private:
	/**
	 * Tag that is associated with the owner of this component. This is used to look up the attribute values inside the
	 * data tables. In this context the last name of the tag is describing the group inside the curve table.
	 *
	 * Example:
	 * NameTag:             Units.LordHouse.Builder
	 * Curve table row key: Builder.Health.MaxHealth
	 */
	UPROPERTY(Category = RTS, BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	FGameplayTag NameTag;
	
	/** Whether this actor can level up.  */
	UPROPERTY(Category = "RTS|Level", BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = true))
	bool bCanLevelUp;

	/** Current level of the actor. */
	UPROPERTY(Category = "RTS|Level", BlueprintReadOnly, EditAnywhere,
			  meta = (AllowPrivateAccess = true, ClampMin = 1, UIMin = 1, EditCondition = bCanLevelUp))
	int32 Level;

	/** Max level of the hero. */
	UPROPERTY(Category = "RTS|Level", EditInstanceOnly,
			  meta = (AllowPrivateAccess = true, ClampMin = 1, UIMin = 1, EditCondition = bCanLevelUp))
	int32 MaxLevel;

	/** Collected XP needed to be on a certain level. */
	UPROPERTY(Category = "RTS|Level", BlueprintReadOnly, EditAnywhere,
			  meta = (AllowPrivateAccess = true, EditCondition = bCanLevelUp))
	FScalableFloat XPPerLevel;

	/** Gameplay effect to fire on level up. */
	UPROPERTY(Category = "RTS|Level", BlueprintReadOnly, EditAnywhere,
			  meta = (AllowPrivateAccess = true, EditCondition = bCanLevelUp))
	TSubclassOf<UGameplayEffect> LevelUpEffect;

	/** Life time collected XP of the actor. */
	UPROPERTY(Category = "RTS|Level", BlueprintReadOnly, VisibleInstanceOnly,
			  meta = (AllowPrivateAccess = true, ClampMin = 1, UIMin = 1))
	float CollectedXP;

	/** XP this actor grants to its killer. The curve table value depends on the level of this actor.  */
	UPROPERTY(Category = "RTS|Level", BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = true))
	FScalableFloat GrantedXP;

	/** Ability points granted on level up and required to increase ability levels. */
	UPROPERTY(Category = "RTS|Level", BlueprintReadOnly, EditAnywhere,
			  meta = (AllowPrivateAccess = true, ClampMin = 1, UIMin = 1, EditCondition = bCanLevelUp))
	int32 AbilityPoints;
};

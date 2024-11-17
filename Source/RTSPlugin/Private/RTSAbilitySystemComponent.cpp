// Craig Duthie 2023

// This Include
#include "RTSAbilitySystemComponent.h"

// Engine Includes
#include <AbilitySystemLog.h>

#include "AbilitySystemGlobals.h"
#include "RTSUseAbilityOrder.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RTSAbilitySystemComponent)

URTSAbilitySystemComponent::URTSAbilitySystemComponent()
{
    Level = 1;
    CollectedXP = 0;
    AbilityPoints = 0;
}

void URTSAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(URTSAbilitySystemComponent, CollectedXP);
    DOREPLIFETIME(URTSAbilitySystemComponent, AbilityPoints);
    //DOREPLIFETIME(URTSAbilitySystemComponent, ItemAbilities);
}

void URTSAbilitySystemComponent::BeginPlay()
{
    Super::BeginPlay();

    OnAbilityEnded.AddUObject(this, &URTSAbilitySystemComponent::AbilityEndedCallback);

    // Initialize the attributes.
    InitializeAttributes(Level, true);

    // Set the current XP based on the initial level.
    if (Level != MaxLevel)
    {
        CollectedXP = GetTotalXPRequiredForLevel(Level - 1);
    }

    else
    {
        CollectedXP = GetTotalXPRequiredForLevel(Level);
    }

    // Initialize the abilities.
    for (TSubclassOf<UGameplayAbility> Ability : InitialAbilities)
    {
        if (Ability != nullptr)
        {
            GiveAbility(FGameplayAbilitySpec((Ability->GetDefaultObject<UGameplayAbility>()), 1));
        }
    }

    bHaveAbilitiesBeenGiven = true;
}

void URTSAbilitySystemComponent::ReceiveDamage(URTSAbilitySystemComponent* SourceASC, float UnmitigatedDamage, float MitigatedDamage)
{
    ReceivedDamageDelegate.Broadcast(SourceASC, UnmitigatedDamage, MitigatedDamage);
}

float URTSAbilitySystemComponent::GetAbilityRange(TSubclassOf<URTSGameplayAbility> Ability)
{
    if (Ability == nullptr)
    {
        return 0.0f;
    }

    for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
    {
        URTSGameplayAbility* AbilityCDO = Ability->GetDefaultObject<URTSGameplayAbility>();
        if (Spec.Ability == AbilityCDO)
        {
            return AbilityCDO->GetRange(Spec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo());
        }
    }

    return 0.0f;
}

void URTSAbilitySystemComponent::GetAutoOrders_Implementation(TArray<FRTSOrderTypeWithIndex>& OutAutoOrders)
{
    // TODO
    TArray<TSubclassOf<UGameplayAbility>> BasicAttackAbilities;// = URTSAbilitySystemHelper::GetBasicAttackAbilities(this);
    TArray<TSubclassOf<UGameplayAbility>> InitialAndUnlockableAbilities = GetInitialAndUnlockableAbilities();

    for (int32 Index = 0; Index < InitialAndUnlockableAbilities.Num(); ++Index)
    {
        TSubclassOf<UGameplayAbility> AbilityType = InitialAndUnlockableAbilities[Index];

        if (AbilityType == nullptr)
        {
            continue;
        }

        URTSGameplayAbility* Ability = AbilityType->GetDefaultObject<URTSGameplayAbility>();
        if (Ability == nullptr)
        {
            continue;
        }

        if (Ability->GetTargetType() != ERTSOrderTargetType::PASSIVE && !BasicAttackAbilities.Contains(AbilityType))
        {
            OutAutoOrders.Add(FRTSOrderTypeWithIndex(UseAbilityOrder, Index));
        }
    }
}

FName URTSAbilitySystemComponent::GetName() const
{
    static FName Name = URTSAbilitySystemHelper::GetLastTagName(NameTag);
    return Name;
}

FGameplayTag URTSAbilitySystemComponent::GetNameTag() const
{
    return NameTag;
}

void URTSAbilitySystemComponent::SetLevel(int32 NewLevel)
{
    AActor* Owner = GetOwner();
    if (Owner == nullptr)
    {
        return;
    }

    if (NewLevel >= MaxLevel)
    {
        CollectedXP = GetTotalXPRequiredForLevel(MaxLevel - 1);
        NewLevel = MaxLevel;
    }

    if (Level == NewLevel)
    {
        return;
    }

    if (Owner->HasAuthority())
    {
        if (NewLevel > 1)
        {
            FGameplayEffectSpecHandle Effect = MakeOutgoingSpec(LevelUpEffect, NewLevel, MakeEffectContext());
            if (Effect.IsValid())
            {
                ApplyGameplayEffectSpecToSelf(*Effect.Data.Get());
            }
        }

        // TODO
        InitializeAttributes(NewLevel, false);

        // Grant ability points.
        SetAbilityPoints(AbilityPoints + NewLevel - Level);
    }

    Level = NewLevel;
}

float URTSAbilitySystemComponent::GetGrantedXP() const
{
    return GrantedXP.GetValueAtLevel(Level);
}

float URTSAbilitySystemComponent::GetTotalXPRequiredForLevel(int32 InLevel) const
{
    float ValueAtLevel = 0;

    for (int32 CurrentLevel = 1; CurrentLevel <= InLevel; ++CurrentLevel)
    {
        ValueAtLevel += XPPerLevel.GetValueAtLevel(CurrentLevel);
    }

    return ValueAtLevel;
}

float URTSAbilitySystemComponent::GetCurrentLevelXP() const
{
    float CurrentLevelXP = Level > 1 ? GetTotalXPRequiredForLevel(Level - 1) : 0;
    return CollectedXP - CurrentLevelXP;
}

float URTSAbilitySystemComponent::GetNextLevelXP() const
{
    return XPPerLevel.GetValueAtLevel(Level);
}

float URTSAbilitySystemComponent::GetCurrentLevelXPProgress() const
{
    float CurrentLevelXP = GetCurrentLevelXP();
    float NextLevelXP = GetNextLevelXP();

    if (NextLevelXP <= 0.0f)
    {
        return 1.0f;
    }

    return CurrentLevelXP / NextLevelXP;
}

float URTSAbilitySystemComponent::GetCollectedXP() const
{
    return CollectedXP;
}

void URTSAbilitySystemComponent::AddCollectedXP(float AdditionalCollectedXP)
{
    if (!bCanLevelUp)
    {
        return;
    }

    if (Level == MaxLevel)
    {
        return;
    }

    float OldCollectedXP = CollectedXP;
    CollectedXP += AdditionalCollectedXP;

    // Notify listers.
   // NotifyOnCollectedXPChanged(OldCollectedXP, CollectedXP);

    //UpdateLevel();
}

bool URTSAbilitySystemComponent::CanLevelUp() const
{
    return bCanLevelUp;
}

int32 URTSAbilitySystemComponent::GetAbilityPoints() const
{
    return AbilityPoints;
}

void URTSAbilitySystemComponent::SetAbilityPoints(int32 NewAbilityPoints)
{
    if (AbilityPoints == NewAbilityPoints)
    {
        return;
    }

    int32 OldAbilityPoints = AbilityPoints;
    AbilityPoints = NewAbilityPoints;

    // TODO
    //NotifyOnAbilityPointsChanged(OldAbilityPoints, NewAbilityPoints);
}

void URTSAbilitySystemComponent::IncreaseAbilityLevel(TSubclassOf<UGameplayAbility> AbilityClass,
    bool bUseAbilityPoint)
{
    if (AbilityClass == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("%s tried to increase the level of an ability, but no ability was specified."),
               *GetOwner()->GetName());
        return;
    }

    // Check available ability points.
    if (bUseAbilityPoint && AbilityPoints <= 0)
    {
        UE_LOG(LogTemp, Error,
               TEXT("%s tried to increase the level of ability %s, but no ability points were available."),
               *GetOwner()->GetName(), *AbilityClass->GetName());
        return;
    }

    // Find ability.
    for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
    {
        if (!IsValid(AbilitySpec.Ability) || AbilitySpec.Ability->GetClass() != AbilityClass)
        {
            continue;
        }

        // Check max level.
        if (AbilitySpec.Level == URTSAbilitySystemHelper::GetAbilityMaxLevel(this, AbilityClass))
        {
            UE_LOG(LogTemp, Error,
                   TEXT("%s tried to increase the level of ability %s above %i, but that ability is already at maximum "
                        "level."),
                   *GetOwner()->GetName(), *AbilityClass->GetName(), AbilitySpec.Level);
            return;
        }

        // Increase level.
        ++AbilitySpec.Level;

        // Notify instances.
        for (UGameplayAbility* Ability : AbilitySpec.GetAbilityInstances())
        {
            URTSGameplayAbility* RTSAbility = Cast<URTSGameplayAbility>(Ability);
            if (Ability->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
            {
                RTSAbility->OnAbilityLevelChanged(AbilitySpec.Level);
            }
        }

        if (bUseAbilityPoint)
        {
            // Remove ability point.
            SetAbilityPoints(AbilityPoints - 1);
        }

        UE_LOG(LogTemp, Log, TEXT("%s increased the level of ability %s to %i. Remaining ability points: %i."),
               *GetOwner()->GetName(), *AbilityClass->GetName(), AbilitySpec.Level, AbilityPoints);

        return;
    }

    if (UnlockableAbilities.Contains(AbilityClass))
    {
        // Grant ability.
        GiveAbility(FGameplayAbilitySpec((AbilityClass->GetDefaultObject<UGameplayAbility>()), 1));

        if (bUseAbilityPoint)
        {
            // Remove ability point.
            SetAbilityPoints(AbilityPoints - 1);
        }

        UE_LOG(LogTemp, Log, TEXT("%s learned new ability %s. Remaining ability points: %i."), *GetOwner()->GetName(),
               *AbilityClass->GetName(), AbilityPoints);

        return;
    }

    UE_LOG(LogTemp, Error, TEXT("%s tried to increase the level of ability %s, but that ability wasn't found."),
           *GetOwner()->GetName(), *AbilityClass->GetName());
}

void URTSAbilitySystemComponent::AddTag(const FGameplayTag Tag)
{
    FGameplayTagContainer Container;
    Container.AddTag(Tag);

    AddTags(Container);
}

void URTSAbilitySystemComponent::RemoveTag(const FGameplayTag Tag)
{
    FGameplayTagContainer Container;
    Container.AddTag(Tag);

    RemoveTags(Container);
}

void URTSAbilitySystemComponent::AddTags(const FGameplayTagContainer& Tags)
{
    const AActor* Owner = GetOwner();
    if (Owner == nullptr)
    {
        return;
    }

    if (!Owner->HasAuthority())
    {
        UE_LOG(LogTemp, Error,
            TEXT("Client tried to add the tags '%s' to '%s'. Tags should only be added on the server."),
            *Tags.ToString(), *Owner->GetName());

        return;
    }

    if (bLogTagChanges)
    {
        for (const FGameplayTag& Tag : Tags)
        {
            if (!HasMatchingGameplayTag(Tag))
            {
                UE_LOG(LogTemp, Log, TEXT("Adding tag %s to %s."), *Tag.ToString(), *Owner->GetName());
            }
        }
    }

    AddLooseGameplayTags(Tags);
    AddMinimalReplicationGameplayTags(Tags);
}

void URTSAbilitySystemComponent::RemoveTags(const FGameplayTagContainer& Tags)
{
    const AActor* Owner = GetOwner();
    if (Owner == nullptr)
    {
        return;
    }

    if (!Owner->HasAuthority())
    {
        UE_LOG(LogTemp, Error,
            TEXT("Client tried to remove the tags '%s' from '%s'. Tags should only be removed on the server."),
            *Tags.ToString(), *Owner->GetName());

        return;
    }

    if (bLogTagChanges)
    {
        for (const FGameplayTag& Tag : Tags)
        {
            if (HasMatchingGameplayTag(Tag))
            {
                UE_LOG(LogTemp, Log, TEXT("Removing tag %s from %s."), *Tag.ToString(), *Owner->GetName());
            }
        }
    }

    RemoveLooseGameplayTags(Tags);
    RemoveMinimalReplicationGameplayTags(Tags);
}

int32 URTSAbilitySystemComponent::GetLevel() const
{
    return Level;
}

TArray<TSubclassOf<UGameplayAbility>> URTSAbilitySystemComponent::GetInitialAndUnlockableAbilities() const
{
    TArray<TSubclassOf<UGameplayAbility>> OutAbilities;

    for (TSubclassOf<UGameplayAbility> InitialAbility : InitialAbilities)
    {
        OutAbilities.Add(InitialAbility);
    }

    for (TSubclassOf<UGameplayAbility> UnlockableAbility : UnlockableAbilities)
    {
        OutAbilities.Add(UnlockableAbility);
    }

    return OutAbilities;
}

int32 URTSAbilitySystemComponent::GetAbilityIndex(TSubclassOf<UGameplayAbility> Ability) const
{
    const TArray<TSubclassOf<UGameplayAbility>>& Abilities = GetInitialAndUnlockableAbilities();

    for (int32 i = 0; i < Abilities.Num(); i++)
    {
        if (Abilities[i] == Ability)
        {
            return i;
        }
    }

    return INDEX_NONE;
}

bool URTSAbilitySystemComponent::TryActivateAbilityByClassWithEventData(TSubclassOf<UGameplayAbility> Ability, const FGameplayEventData& EventData, bool bAllowRemoteActivation /*= false*/)
{
    if (FGameplayAbilitySpec* Spec = FindAbilitySpecFromClass(Ability))
    {
        return TryActivateAbilityWithEventData(Spec->Handle, EventData, bAllowRemoteActivation);
    }

    return false;
}

bool URTSAbilitySystemComponent::TryActivateAbilityWithEventData(FGameplayAbilitySpecHandle AbilityToActivate, const FGameplayEventData& EventData, bool bAllowRemoteActivation /*= false*/)
{
    FGameplayTagContainer FailureTags;
    FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilityToActivate);
    if (!Spec)
    {
        ABILITY_LOG(Warning, TEXT("TryActivateAbility called with invalid Handle"));
        return false;
    }

    UGameplayAbility* Ability = Spec->Ability;

    if (!Ability)
    {
        ABILITY_LOG(Warning, TEXT("TryActivateAbility called with invalid Ability"));
        return false;
    }

    const FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();

    // make sure the ActorInfo and then Actor on that FGameplayAbilityActorInfo are valid, if not bail out.
    if (ActorInfo == nullptr || !ActorInfo->OwnerActor.IsValid() || !ActorInfo->AvatarActor.IsValid())
    {
        return false;
    }


    const ENetRole NetMode = ActorInfo->AvatarActor->GetLocalRole();

    // This should only come from button presses/local instigation (AI, etc).
    if (NetMode == ROLE_SimulatedProxy)
    {
        return false;
    }

    bool bIsLocal = AbilityActorInfo->IsLocallyControlled();

    // Check to see if this a local only or server only ability, if so either remotely execute or fail
    if (!bIsLocal && (Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalOnly || Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted))
    {
        //--- We should never use remote activation
        // if (bAllowRemoteActivation)
        // {
        //     ClientTryActivateAbility(AbilityToActivate);
        //     return true;
        // }

        ABILITY_LOG(Log, TEXT("Can't activate LocalOnly or LocalPredicted ability %s when not local."), *Ability->GetName());
        return false;
    }

    if (NetMode != ROLE_Authority && (Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::ServerOnly || Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::ServerInitiated))
    {
        //--- We should never use remote activation
        // if (bAllowRemoteActivation)
        // {
        //     if (Ability->CanActivateAbility(AbilityToActivate, ActorInfo, nullptr, nullptr, &FailureTags))
        //     {
        //         // No prediction key, server will assign a server-generated key
        //         CallServerTryActivateAbility(AbilityToActivate, Spec->InputPressed, FPredictionKey());
        //         return true;
        //     }
        //     else
        //     {
        //         NotifyAbilityFailed(AbilityToActivate, Ability, FailureTags);
        //         return false;
        //     }
        // }

        ABILITY_LOG(Log, TEXT("Can't activate ServerOnly or ServerInitiated ability %s when not the server."), *Ability->GetName());
        return false;
    }

    return InternalTryActivateAbility(AbilityToActivate, FPredictionKey(), nullptr, nullptr, &EventData);
}

void URTSAbilitySystemComponent::InitializeAttributes(int AttributeLevel, bool bInitialInit)
{
    if (!NameTag.IsValid())
    {
        return;
    }

    if (DefaultStartingData.Num() == 0)
    {
        return;
    }

    FName GroupName = URTSAbilitySystemHelper::GetLastTagName(NameTag);

    // TODO Craig
    // Note that this might cause a crash when no valid paths to data tables where specified in the 'Game.ini' file.
    // FAttributeSetInitter* AttributeInitter = UAbilitySystemGlobals::Get().GetAttributeSetInitter();
    //
    // // This is a work around for a bug that happens at least in the editor. It might be that the 'SpawnedAttributes'
    // // contains nullptr entries for some for some unknown reason. This has properly something to do with serialization.
    // // 'AttributeInitter->InitAttributeSetDefaults' will crash when the array contains a nullptr.
    // //for (int32 i = SpawnedAttributes.Num() - 1; i >= 0; --i)
    // //{
    // //    if (SpawnedAttributes[i] == nullptr)
    // //    {
    // //        SpawnedAttributes.RemoveAt(i);
    // //    }
    // //}
    //
    // UE_LOG(LogTemp, Verbose, TEXT("Initializing attributes of %s with group name %s..."), *GetOwner()->GetName(),
    //        *GroupName.ToString());
    //
    // AttributeInitter->InitAttributeSetDefaults(this, GroupName, AttributeLevel, bInitialInit);

    //for (UAttributeSet* AttributeSet : SpawnedAttributes)
    //{
    //    URTSAttributeSet* RTSAttributeSet = Cast<URTSAttributeSet>(AttributeSet);
    //    if (RTSAttributeSet != nullptr)
    //    {
    //        RTSAttributeSet->PostInitializeProperties(bInitialInit);
    //    }
    //}
}

void URTSAbilitySystemComponent::AbilityEndedCallback(const FAbilityEndedData& AbilityEndedData)
{
    OnGameplayAbilityEnded.Broadcast(AbilityEndedData.AbilityThatEnded->GetClass(), AbilityEndedData.AbilitySpecHandle,
        AbilityEndedData.bWasCancelled);
}

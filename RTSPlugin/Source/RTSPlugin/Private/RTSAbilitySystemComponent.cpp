// Craig Duthie 2023

// This Include
#include "RTSAbilitySystemComponent.h"

// Engine Includes
#include <AbilitySystemLog.h>

#include UE_INLINE_GENERATED_CPP_BY_NAME(RTSAbilitySystemComponent)

void URTSAbilitySystemComponent::BeginPlay()
{
    Super::BeginPlay();

    OnAbilityEnded.AddUObject(this, &URTSAbilitySystemComponent::AbilityEndedCallback);

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

//TArray<TSubclassOf<UGameplayAbility>> URTSAbilitySystemComponent::GetInitialAndUnlockableAbilities() const
//{
//	return ActivatableAbilities;
//}

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

void URTSAbilitySystemComponent::AbilityEndedCallback(const FAbilityEndedData& AbilityEndedData)
{
    OnGameplayAbilityEnded.Broadcast(AbilityEndedData.AbilityThatEnded->GetClass(), AbilityEndedData.AbilitySpecHandle,
        AbilityEndedData.bWasCancelled);
}

// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "achievements_manager_internal.h"
#include "xbox_live_context_internal.h"
#include "real_time_activity_manager.h"

using namespace xbox::services;

XblAchievementsManagerResult::XblAchievementsManagerResult(_In_ const XblAchievement & achievement)
    : m_achievements({ achievement }),
    m_achievementsData(nullptr),
    m_achievementsCount(),
    m_explicitCleanup(false)
{
}

XblAchievementsManagerResult::XblAchievementsManagerResult(_In_ Vector<XblAchievement>& achievements, _In_ bool explicitCleanup)
    : m_achievements(std::move(achievements)),
    m_achievementsData(m_achievements.data()),
    m_achievementsCount(m_achievements.size()),
    m_explicitCleanup(explicitCleanup)
{
}

XblAchievementsManagerResult::XblAchievementsManagerResult(_In_ XblAchievement* achievements, _In_ size_t achievementCount, _In_ bool explicitCleanup)
    : m_achievements(achievements, achievements + achievementCount),
    m_achievementsData(achievements),
    m_achievementsCount(achievementCount),
    m_explicitCleanup(explicitCleanup)
{
}

XblAchievementsManagerResult::~XblAchievementsManagerResult()
{
    for (auto& achievement : m_achievements)
    {
        achievements::manager::AchievementsManager::CleanUpAchievementCopyForResult(achievement);
    }
    if (m_explicitCleanup)
    {
        DeleteArray<XblAchievement>(m_achievementsData, m_achievementsCount);
    }
    m_achievementsData = nullptr;
    m_achievementsCount = 0;
    m_achievements.clear();
}

const Vector<XblAchievement>& XblAchievementsManagerResult::Achievements() const
{
    return m_achievements;
}

std::shared_ptr<xbox::services::RefCounter> XblAchievementsManagerResult::GetSharedThis()
{
    return shared_from_this();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_MANAGER_CPP_BEGIN

AchievementsManagerUser::AchievementsManagerUser(
    _In_ User&& localUser,
    _In_ const TaskQueue& queue
) noexcept :
    m_xuid{ localUser.Xuid() },
    m_rtaManager{ GlobalState::Get()->RTAManager() },
    m_queue{ queue.DeriveWorkerQueue() }
{
    // Maintain legacy RTA activation count.
    m_rtaManager->Activate(localUser);

    m_xblContext = XblContext::Make(std::move(localUser));
}

AchievementsManagerUser::~AchievementsManagerUser()
{
    // Terminate any background work
    m_queue.Terminate(false);

    // Unregister rta handlers
    if (m_achievementProgressToken)
    {
        m_xblContext->AchievementsService()->RemoveAchievementProgressChangeHandler(m_achievementProgressToken);
    }
    if (m_rtaResyncToken)
    {
        m_rtaManager->RemoveResyncHandler(m_xblContext->User(), m_rtaResyncToken);
    }
    if (m_rtaConnectionToken)
    {
        m_rtaManager->RemoveStateChangedHandler(m_xblContext->User(), m_rtaConnectionToken);
    }

    m_rtaManager->Deactivate(m_xblContext->User());

    for (auto& pair : m_userAchievements)
    {
        AchievementsManager::CleanUpDeepCopyAchievement(*pair.second);
    }
    DeleteArray(m_achievementCache, m_userAchievements.size());
}

Result<void> AchievementsManagerUser::Initialize(
    _In_ AsyncContext<HRESULT> async
)
{
    assert(!m_isInitialized);
    RETURN_HR_IF_FAILED(m_xblContext->Initialize(m_rtaManager));
    m_xblContext->Settings()->SetHttpUserAgent(HttpCallAgent::AchievementsManager);

    std::weak_ptr<AchievementsManagerUser> weakThis{ shared_from_this() };
    m_achievementProgressToken = m_xblContext->AchievementsService()->AddAchievementProgressChangeHandler(
        [weakThis](const XblAchievementProgressChangeEventArgs& args)
        {
            auto sharedThis{ weakThis.lock() };
            if (sharedThis)
            {
                // convert to manager event
                for (uint32_t entryIndex = 0; entryIndex < args.entryCount; ++entryIndex)
                {
                    XblAchievementsManagerEvent achievementEvent;
                    achievementEvent.xboxUserId = sharedThis->m_xuid;
                    achievementEvent.eventType = XblAchievementsManagerEventType::AchievementProgressUpdated;
                    
                    // Make a deep copy of the achievement entries here. Can't steal them with move,
                    //  as other handlers might have been added.
                    achievementEvent.progressInfo = AchievementProgressChangeSubscription::DeepCopyProgressChangeEntry(
                        args.updatedAchievementEntries[entryIndex]
                    );

                    // Achievement progress notifications can potentially come in with no 
                    //  targetProgressValue defined if it does not result in an achievement 
                    //  being unlocked, so we need to copy the targetProgressValue from 
                    //  the cached achievement.
                    auto& progression = achievementEvent.progressInfo.progression;
                    for (uint64_t i = 0; i < progression.requirementsCount; ++i)
                    {
                        auto& requirement = progression.requirements[i];
                        if (requirement.targetProgressValue == nullptr)
                        {
                            auto& cachedAchievement = sharedThis->m_userAchievements[achievementEvent.progressInfo.achievementId];
                            auto& cachedProgression = cachedAchievement->progression;

                            auto matchedRequirement = std::find_if(cachedProgression.requirements, cachedProgression.requirements +cachedProgression.requirementsCount,
                                [&requirement](const XblAchievementRequirement& cachedRequirement)->bool
                                {
                                    return utils::str_icmp(requirement.id, cachedRequirement.id);
                                });
                            if (matchedRequirement == (cachedProgression.requirements + cachedProgression.requirementsCount))
                            {
                                char errorMsg[1024];
                                SPRINTF(errorMsg, 1024, "Could not find a requirement with ID %s for cached achievement %s. Couldn't populate event with targetProgressValue.", requirement.id, achievementEvent.progressInfo.achievementId);
                                LOGS_ERROR << errorMsg;
                            }
                            requirement.targetProgressValue = Make(matchedRequirement->targetProgressValue);
                        }
                    }

                    {
                        std::lock_guard<std::mutex> lock{ sharedThis->m_mutex };
                        sharedThis->m_eventsToProcess.push_back(achievementEvent);
                    }
                }

            }
        }
    );
    
    auto resyncCallback = [weakThis] (HRESULT hr)
    {
        auto sharedThis{ weakThis.lock() };
        auto state{ GlobalState::Get() };
        if (sharedThis && state)
        {
            if (FAILED(hr))
            {
                LOGS_ERROR << "Fetching state of achievements for user with ID " << sharedThis->Xuid() << " for RTA Resync failed with error code " << hr;
            }
        }
    };

    // rta resync
    m_rtaResyncToken = m_rtaManager->AddResyncHandler(m_xblContext->User(),
        [weakThis, resyncCallback]()
        {
            if (auto sharedThis{ weakThis.lock() })
            {
                sharedThis->FetchAchievements(AsyncContext<HRESULT> {
                    sharedThis->m_queue,
                    resyncCallback
                });
            }
        }
    );

    m_rtaConnectionToken = m_rtaManager->AddStateChangedHandler(m_xblContext->User(),
        [weakThis, resyncCallback](XblRealTimeActivityConnectionState connectionState)
        {
            // Only take action if rta just connected.
            if (connectionState != XblRealTimeActivityConnectionState::Connected)
            {
                return;
            }

            if (auto sharedThis{ weakThis.lock() })
            {
                // We only want to refetch state data if we're already initialized,
                //  otherwise there is already a fetch happening.
                if (!sharedThis->m_isInitialized)
                {
                    return;
                }

                // If this function hasn't returned yet, this means that the
                //  RTA connection dropped at some point (whether the network 
                //  connection dropped, or an issue with the RTA service, 
                //  etc.) and we've missed some data, meaning we need to 
                //  resync the cached state from the service.
                sharedThis->FetchAchievements(AsyncContext<HRESULT> {
                    sharedThis->m_queue,
                    resyncCallback
                });
            }
        }
    );

    return FetchAchievements(AsyncContext<HRESULT> {
        m_queue,
            [
                async,
                weakThis
            ]
        (HRESULT hr)
        {
            auto sharedThis{ weakThis.lock() };
            auto state{ GlobalState::Get() };
            if (sharedThis && state)
            {
                if (SUCCEEDED(hr))
                {
                    sharedThis->m_isInitialized = true;
                }
                async.Complete(hr);
            }
        }
    });
}

bool AchievementsManagerUser::IsInitialized() const
{
    return m_isInitialized;
}

uint64_t AchievementsManagerUser::Xuid() const
{
    return m_xuid;
}

Result<XblAchievement> AchievementsManagerUser::GetAchievement(_In_ const String & id)
{
    if (m_userAchievements.find(id) == m_userAchievements.end())
    {
        char errorMsg[1024];
        SPRINTF(errorMsg, 1024, "Cannot find achievement with ID %s for user with ID %llu.", id.c_str(), static_cast<unsigned long long>(m_xuid));
        return { E_BOUNDS, errorMsg };
    }

    std::lock_guard<std::mutex> lock{ m_mutex };
    return AchievementsManager::CopyAchievementForResult(*m_userAchievements[id]);
}

XblAchievement* AchievementsManagerUser::GetAchievements() 
{
    std::lock_guard<std::mutex> lock{ m_mutex };
    return AchievementsManager::DeepCopyAchievements(m_achievementCache, m_userAchievements.size());
}

Vector<XblAchievement> AchievementsManagerUser::GetAchievements(
    _In_ AchievementsManagerSortFilterSettings sortFilterSettings
)
{
    Vector<XblAchievement> filteredAchievements;
    
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        for (const auto& pair : m_userAchievements)
        {
            const XblAchievement& achievement = *pair.second;
            bool isIncluded = true;
            
            // Does this achievement match the state requested?
            switch (sortFilterSettings.stateFilter)
            {
            case AchievementsManagerFilterType::Unlocked:
                isIncluded &= achievement.progressState == XblAchievementProgressState::Achieved;
                break;
            case AchievementsManagerFilterType::InProgress:
                isIncluded &= achievement.progressState == XblAchievementProgressState::InProgress;
                break;
            case AchievementsManagerFilterType::NotStarted:
                isIncluded &= achievement.progressState == XblAchievementProgressState::NotStarted;
                break; 
            default:
                break; // Don't need to &= true here, it's just a wasted op.
            }

            if (isIncluded)
            {
                // Pushing back a shallow copy here, in case we need to truncate
                //  the list before we return it.
                filteredAchievements.push_back(achievement);
            }
        }
    }
    
    // We only sort on UnlockTime, since we don't offer achievements from multiple
    //  titles. This would need to change if we ever give more options for values to
    //  sort on.
    if (sortFilterSettings.sortBy == XblAchievementOrderBy::UnlockTime)
    {
        // call sort function
        if (sortFilterSettings.sortOrder == XblAchievementsManagerSortOrder::Ascending)
        {
            std::sort(filteredAchievements.begin(), filteredAchievements.end(), 
                [](const XblAchievement& a, const XblAchievement& b) -> bool
                {
                    return a.progression.timeUnlocked < b.progression.timeUnlocked;
                }
            );
        }
        else if (sortFilterSettings.sortOrder == XblAchievementsManagerSortOrder::Descending)
        {
            std::sort(filteredAchievements.begin(), filteredAchievements.end(),
                [](const XblAchievement& a, const XblAchievement& b) -> bool
                {
                    return a.progression.timeUnlocked > b.progression.timeUnlocked;
                }
            );
        }
    }
    return AchievementsManager::DeepCopyAchievements(filteredAchievements);
}

uint64_t AchievementsManagerUser::GetAchievementCount() const
{
    return m_userAchievements.size();
}

Result<void> AchievementsManagerUser::CanUpdateAchievement(_In_ const String & achievementId, _In_ uint8_t progress)
{
    if (m_userAchievements.find(achievementId) == m_userAchievements.end())
    {
        char errorMsg[1024];
        SPRINTF(errorMsg, 1024, "Requested achievement with ID %s doesn't exist.", achievementId.c_str());
        return { E_BOUNDS, errorMsg };
    }
    if (m_userAchievements[achievementId]->progressState == XblAchievementProgressState::Achieved)
    {
        char errorMsg[1024];
        SPRINTF(errorMsg, 1024, "Requested achievement with ID %s already achieved.", achievementId.c_str());
        return{ E_UNEXPECTED, errorMsg };
    }
    if (m_userAchievements[achievementId]->progression.requirementsCount > 1)
    {
        char errorMsg[1024];
        SPRINTF(errorMsg, 1024, "Requested achievement with ID %s is an event based achievement and can't be updated through AchievementManager. Use the Stats API instead.", achievementId.c_str());
        return { E_NOT_SUPPORTED, errorMsg };
    }

    uint32_t targetValue = xbox::services::utils::internal_string_to_uint32(m_userAchievements[achievementId]->progression.requirements[0].targetProgressValue);
    if (targetValue != 100)
    {
        char errorMsg[1024];
        SPRINTF(errorMsg, 1024, "Requested achievement with ID %s is an event based achievement and can't be updated through AchievementManager. Use the Stats API instead.", achievementId.c_str());
        return { E_NOT_SUPPORTED, errorMsg };
    }

    uint32_t currentValue = xbox::services::utils::internal_string_to_uint32(m_userAchievements[achievementId]->progression.requirements[0].currentProgressValue);
    if (currentValue >= progress)
    {
        char errorMsg[1024];
        SPRINTF(errorMsg, 1024, "Can't update achievement with ID %s because the new progress value of %u is less than or equal to the achievement's current progress value of %u.",
            achievementId.c_str(),
            progress,
            currentValue
        );
        return { E_INVALIDARG, errorMsg };
    }
    return S_OK;
}

Result<void> AchievementsManagerUser::UpdateAchievement(_In_ const String& achievementId, _In_ uint8_t percent)
{
    return m_xblContext->AchievementsService()->UpdateAchievement(
        m_xuid,
        achievementId,
        percent,
        AsyncContext<Result<void>> {
        [
            achievementId, 
            percent
        ]
    (Result<void> result)
    {
        if (FAILED(result.Hresult()))
        {
            LOGS_ERROR << "Updating achievement " << achievementId << " to " << percent << "% complete failed with error code " << result.Hresult();
        }
    }
    });
}

enum class ProgressValueType : uint32_t
{
    NonNumeric = 0,
    UnsignedLong,
    SignedLong,
    FloatingPoint
};

ProgressValueType IsNumber(const char* str)
{
    char* p = nullptr;
    
    (void)strtoul(str, &p, 0);
    if (p == nullptr)
    {
        return ProgressValueType::UnsignedLong;
    }

    // If it couldn't be represented as an unsigned long, check to see if it can be signed next.
    (void)strtol(str, &p, 0);
    if (p == nullptr)
    {
        return ProgressValueType::SignedLong;
    }
    
    // If neither of those, then either it is a floating point number, or it is non-numeric.
    (void)strtod(str, &p);
    if (p == nullptr)
    {
        return ProgressValueType::FloatingPoint;
    }

    return ProgressValueType::NonNumeric;
}

bool TryParseToDouble(const char* str, double* out)
{
    char* p = nullptr;
    double parsedValue = 0.0;
    parsedValue = strtod(str, &p);
    if (out)
    {
        *out = parsedValue;
    }
    return p == nullptr;
}

bool TryParseToUnsignedLong(const char* str, unsigned long* out)
{
    char* p = nullptr;
    unsigned long parsedValue = 0;
    parsedValue = strtoul(str, &p, 0);
    if (out)
    {
        *out = parsedValue;
    }
    return p == nullptr;
}

bool TryParseToLong(const char* str, long* out)
{
    char* p = nullptr;
    long parsedValue = 0;
    parsedValue = strtol(str, &p, 0);
    if (out)
    { 
        *out = parsedValue;
    }
    return p == nullptr;
}

unsigned long ForceProgressValueToUnsignedLong(const char* progressValue, ProgressValueType valueType)
{
    unsigned long forcedValue = 0;
    switch (valueType)
    {
    case ProgressValueType::UnsignedLong:
        TryParseToUnsignedLong(progressValue, &forcedValue);
        break;
    case ProgressValueType::SignedLong:
    {
        long parsedValue = 0;
        TryParseToLong(progressValue, &parsedValue);
        forcedValue = parsedValue > 0 ? parsedValue : 0;
        break;
    }
    case ProgressValueType::FloatingPoint:
    {
        double parsedValue = 0.0;
        TryParseToDouble(progressValue, &parsedValue);
        forcedValue = static_cast<unsigned long>(parsedValue);
        break;
    }
    default:
        break;
    }
    return forcedValue;
}

bool ShouldUpdateProgress(const char* eventProgressValue, const char* cachedProgressValue)
{
    ProgressValueType eventValueType = IsNumber(eventProgressValue); 
    ProgressValueType cachedValueType = IsNumber(cachedProgressValue);

    // Always update if both values are non numeric, as we don't know how to do that comparison.
    // Maybe add a way for the title to add a conversion handler(s) in case they use non-numbers
    //  to represent achievement progress?
    if (eventValueType == cachedValueType && eventValueType == ProgressValueType::NonNumeric)
    {
        LOGS_WARN << "Comparing progress values that are string typed. Cannot determine which string is considered greater by the title, and will always result in updating to cached value";
        return true;
    }
    else if (eventValueType == ProgressValueType::NonNumeric || cachedValueType == ProgressValueType::NonNumeric)
    {
        // Only one of the two progress values is numeric, so we don't know how to do that 
        //  conversion. In this case we should log an error and not update, as this is 
        //  likely an unintended result.
        LOGS_ERROR << "Cannot compare numeric event progress value against cached non-numeric progress value. Progress for this requirement will not be updated.";
        return false;
    }
    else if (eventValueType != cachedValueType)
    {
        // Cached and event progress values are represented by different numeric types, so log 
        //  a warning and continue. Going to cast all values to unsigned long to continue.
        LOGS_WARN << "Comparing two different numeric types. Both values will be cast to unsigned long before comparison.";
        return ForceProgressValueToUnsignedLong(eventProgressValue, eventValueType) > ForceProgressValueToUnsignedLong(cachedProgressValue, cachedValueType);
    }

    // Value types are the same.
    bool shouldUpdate = false;
    switch (cachedValueType)
    {
    case ProgressValueType::UnsignedLong:
    {
        unsigned long eventValue, cachedValue; 
        TryParseToUnsignedLong(cachedProgressValue, &cachedValue);
        TryParseToUnsignedLong(eventProgressValue, &eventValue);

        shouldUpdate = eventValue > cachedValue;
        break;
    }
    case ProgressValueType::SignedLong:
    {
        long eventValue, cachedValue;
        TryParseToLong(cachedProgressValue, &cachedValue);
        TryParseToLong(eventProgressValue, &eventValue);

        shouldUpdate = eventValue > cachedValue;
        break;
    }
    case ProgressValueType::FloatingPoint:
    {
        double eventValue, cachedValue;
        TryParseToDouble(cachedProgressValue, &cachedValue);
        TryParseToDouble(eventProgressValue, &eventValue);

        shouldUpdate = eventValue > cachedValue;
        break;
    }
    default:
        break;
    }

    return shouldUpdate;
}

Vector<XblAchievementsManagerEvent> AchievementsManagerUser::ProcessEvents()
{
    Vector<XblAchievementsManagerEvent> eventsToApply;
    {
        std::lock_guard<std::mutex> lock{ m_mutex };
        eventsToApply = std::move(m_eventsToProcess);
        eventsToApply.insert(eventsToApply.end(), m_generatedEvents.begin(), m_generatedEvents.end());
        m_generatedEvents.clear();
    }

    // Using a regular for-loop here since we are modifying the contents of the 
    //  vector while iterating over it. Using a range-based for loop or using
    //  iterators would result in the loop ending before the additional members 
    //  are iterated over.
    for (uint32_t i = 0; i < eventsToApply.size(); ++i)
    {
        const XblAchievementsManagerEvent& achievementEvent = eventsToApply[i];
        switch (achievementEvent.eventType)
        {
        case XblAchievementsManagerEventType::AchievementProgressUpdated:
        {
            bool createUnlockEvent = false;

            const XblAchievementProgression& eventProgression = achievementEvent.progressInfo.progression;
            XblAchievementProgression& cachedProgression = m_userAchievements[achievementEvent.progressInfo.achievementId]->progression;
            
            // Explicitly not setting unlock time here, since if this is just getting achieved, we'll have an
            //  unlock event to handle that.

            // Only set the state of the achievement if the event makes the state "InProgress", and the 
            //  achievement wasn't already completed.
            if (m_userAchievements[achievementEvent.progressInfo.achievementId]->progressState != XblAchievementProgressState::Achieved
                && achievementEvent.progressInfo.progressState == XblAchievementProgressState::InProgress)
            {
                m_userAchievements[achievementEvent.progressInfo.achievementId]->progressState = achievementEvent.progressInfo.progressState;
            }

            // Shortcut for if there is only one requirement in each to avoid the logic for the loop.            
            if (cachedProgression.requirementsCount == 1)
            {
                // Check to make sure that the IDs are the same, just to be sure.
                if (!utils::str_icmp(cachedProgression.requirements[0].id, eventProgression.requirements[0].id))
                {
                    char errorMsg[1024];
                    SPRINTF(errorMsg, 1024, "Achievement event for achievement with ID %s has a requirement with ID %s that doesn't exist in the cached achievement.",
                        achievementEvent.progressInfo.achievementId,
                        eventProgression.requirements[0].id
                    );
                    LOGS_ERROR << String(errorMsg);
                }

                // Perform comparison. This can change if service implementation simplifies getting cumulative progress values back.
                if(ShouldUpdateProgress(eventProgression.requirements[0].currentProgressValue, cachedProgression.requirements[0].currentProgressValue))
                {
                    Delete(cachedProgression.requirements[0].currentProgressValue);
                    cachedProgression.requirements[0].currentProgressValue = Make(eventProgression.requirements[0].currentProgressValue);

                    createUnlockEvent = (achievementEvent.progressInfo.progressState == XblAchievementProgressState::Achieved
                        || (String)cachedProgression.requirements[0].currentProgressValue == (String)cachedProgression.requirements[0].targetProgressValue);
                }
            }
            else
            {
                bool allRequirementsComplete = true;
                Vector<XblAchievementRequirement> cachedRequirements(cachedProgression.requirements, cachedProgression.requirements + cachedProgression.requirementsCount);
                for (uint32_t requirementIndex = 0; requirementIndex < eventProgression.requirementsCount; ++requirementIndex)
                {
                    // Find the cached requirement whose ID matches the requirement that was updated.
                    auto requirement = std::find_if(cachedRequirements.begin(), cachedRequirements.end(),
                        [&eventProgression, requirementIndex](const XblAchievementRequirement& requirement)->bool
                        {
                            return utils::str_icmp(requirement.id, eventProgression.requirements[requirementIndex].id) == 0;
                        }
                    );

                    if (requirement == cachedRequirements.end())
                    {
                        char errorMsg[1024];
                        SPRINTF(errorMsg, 1024, "Achievement event for achievement with ID %s has a requirement with ID %s that doesn't exist in the cached achievement.",
                            achievementEvent.progressInfo.achievementId,
                            eventProgression.requirements[requirementIndex].id
                        );
                        LOGS_ERROR << String(errorMsg);
                    }
                    else
                    {
                        // This check can change to simple eventProgress > cachedProgress check if service implementation simplifies 
                        //  getting cumulative progress values back.
                        if (ShouldUpdateProgress(eventProgression.requirements[requirementIndex].currentProgressValue, requirement->currentProgressValue))
                        {
                            requirement->currentProgressValue = eventProgression.requirements[requirementIndex].currentProgressValue;
                        }
                        allRequirementsComplete &= (String)requirement->currentProgressValue == (String)requirement->targetProgressValue;
                    }
                }
                createUnlockEvent = (allRequirementsComplete || achievementEvent.progressInfo.progressState == XblAchievementProgressState::Achieved);
            }

            if (createUnlockEvent)
            {
                XblAchievementsManagerEvent unlockEvent;
                unlockEvent.eventType = XblAchievementsManagerEventType::AchievementUnlocked;
                unlockEvent.xboxUserId = m_xuid;
                unlockEvent.progressInfo.progressState = XblAchievementProgressState::Achieved;
                unlockEvent.progressInfo.achievementId = Make(achievementEvent.progressInfo.achievementId);

                // Only populating the time unlocked as the actual progress info is irrelevent, since it just boils down to 100%
                //  the title can grab that info if it needs it from the preceeding progress update event.
                unlockEvent.progressInfo.progression = XblAchievementProgression{ 0 };
                unlockEvent.progressInfo.progression.timeUnlocked = achievementEvent.progressInfo.progression.timeUnlocked;
                eventsToApply.push_back(unlockEvent);
            }
            break;
        }
        case XblAchievementsManagerEventType::AchievementUnlocked:
        {
            auto achievementId = achievementEvent.progressInfo.achievementId;
            XblAchievement& cachedAchievement = *m_userAchievements[achievementId];
            if (cachedAchievement.progression.requirementsCount != achievementEvent.progressInfo.progression.requirementsCount)
            {
                LOGS_ERROR << "Achievement event for achievement with ID " << achievementId << " has a different number of requirements than the cached version of the achievement.";
            }

            cachedAchievement.progressState = achievementEvent.progressInfo.progressState;
            cachedAchievement.progression.timeUnlocked = achievementEvent.progressInfo.progression.timeUnlocked;
            // For each reuqirement, update the current value to the target value.
            for (uint32_t requirementIndex = 0; requirementIndex < cachedAchievement.progression.requirementsCount; ++requirementIndex)
            {
                // Clean up expects currentProgressValue to be a dynamically allocated string,
                //  so dynamically allocate this too so we don't crash trying to Delete a 
                //  statically allocated literal.
                Delete(cachedAchievement.progression.requirements[requirementIndex].currentProgressValue);
                cachedAchievement.progression.requirements[requirementIndex].currentProgressValue = Make("100");
            }
            break;
        }
        default:
            break;
        }
    }

    return eventsToApply;
}

Vector<XblAchievementsManagerEvent> GenerateEventFromAchievementDiff(uint64_t xuid, const XblAchievement& cached, const XblAchievement& updated)
{
    Vector<XblAchievementsManagerEvent> generatedEvents;

    // Check each requirement on the achievement, and if there is a difference in the progress value,
    //  then generate a progress event.
    Vector<XblAchievementRequirement> updatedRequirements;

    uint64_t requirementCount = updated.progression.requirementsCount;
    for (uint64_t requirementIndex = 0; requirementIndex < requirementCount; ++requirementIndex)
    {
        Vector<XblAchievementRequirement> cachedRequirements(cached.progression.requirements, cached.progression.requirements + cached.progression.requirementsCount);

        auto& updatedRequirement = updated.progression.requirements[requirementIndex];
        
        // Find the cached requirement whose ID matches the requirement that was updated.
        auto cachedRequirement = std::find_if(cachedRequirements.begin(), cachedRequirements.end(),
            [&updatedRequirement](const XblAchievementRequirement& requirement)->bool
            {
                return utils::str_icmp(requirement.id, updatedRequirement.id) == 0;
            }
        );
        if (cachedRequirement == cachedRequirements.end())
        {
            char errorMsg[1024];
            SPRINTF(errorMsg, 1024, "Achievement from achievement event with ID %s has a requirement with ID %s that doesn't exist in the cached achievement.",
                updated.id,
                updatedRequirement.id
            );
            LOGS_ERROR << String(errorMsg);
        }

        // Compare the progress values.
        if (utils::internal_string_to_uint32(cachedRequirement->currentProgressValue) <
            utils::internal_string_to_uint32(updatedRequirement.currentProgressValue))
        {
            updatedRequirements.push_back(updatedRequirement);
        }
    }

    if (updatedRequirements.size() > 0)
    {
        // check to see if we have an event for this change already.
        XblAchievementsManagerEvent event;
        event.xboxUserId = xuid;
        event.eventType = XblAchievementsManagerEventType::AchievementProgressUpdated;
        XblAchievementProgressChangeEntry& progressEntry = event.progressInfo;
        progressEntry.progressState = updated.progressState;
        progressEntry.achievementId = Make(updated.id);
        XblAchievementProgression& progress = progressEntry.progression;

        
        // Move the contents of the vector into the progression struct.
        progress.requirementsCount = updatedRequirements.size();
        progress.requirements = MakeArray<XblAchievementRequirement>(progress.requirementsCount);
        for (uint64_t i = 0; i < progress.requirementsCount; ++i)
        {
            std::swap(progress.requirements[i].id, updatedRequirements[i].id);
            std::swap(progress.requirements[i].currentProgressValue, updatedRequirements[i].currentProgressValue);
            std::swap(progress.requirements[i].targetProgressValue, updatedRequirements[i].targetProgressValue);
        }
        progress.timeUnlocked = updated.progression.timeUnlocked;
        
        generatedEvents.push_back(event);
    }

    return generatedEvents;
}

HRESULT AchievementsManagerUser::FetchAchievements(_In_ AsyncContext<HRESULT> async)
{
    constexpr uint32_t achievementsPerFetch = 100;

    return m_xblContext->AchievementsService()->GetAchievementsForTitle(
        m_xuid,
        AppConfig::Instance()->TitleId(),
        XblAchievementType::All,
        false,
        XblAchievementOrderBy::DefaultOrder,
        0,
        achievementsPerFetch,
        AsyncContext<Result<std::shared_ptr<XblAchievementsResult>>>{ async.Queue(),
        [
            =,
            weakThis = std::weak_ptr<AchievementsManagerUser>{ shared_from_this() }
        ]
    (Result<std::shared_ptr<XblAchievementsResult>> result)
    {
        auto sharedThis{ weakThis.lock() };
        if (sharedThis)
        {
            sharedThis->HandleAchievementsResults(result, achievementsPerFetch, async);
        }
    }
    });
}

HRESULT AchievementsManagerUser::HandleAchievementsResults(
    _In_ Result<std::shared_ptr<XblAchievementsResult>> result,
    _In_ uint32_t achievementsPerFetch,
    _In_ AsyncContext<HRESULT> async,
    _In_ Vector<XblAchievement> fetchedAchievements
)
{
    if (Succeeded(result))
    {
        {
            std::lock_guard<std::mutex> lock{ m_mutex };
            for (const XblAchievement& achievement : result.Payload()->Achievements())
            {
                // If this is being called after the user has already been initialized, then
                //  something happened and needed to do an RTA resync. So compare against the
                //  existing achievement, and generate the proper event from it.
                if (m_isInitialized)
                {
                    // We only want to have this function generate unlock events if we're expecting 
                    //  RTA notifications. Otherwise it will be produced from the progress change
                    //  event.
                    auto generatedEvents = GenerateEventFromAchievementDiff(m_xuid, *m_userAchievements[achievement.id], achievement);
                    if (generatedEvents.size() > 0)
                    {
                        for (auto generatedEvent : generatedEvents)
                        {
                            // Check to see if there is already an event for this change. Only add this event
                            //  to the generated list if it is completely unique.
                            auto foundEvent = std::find_if(m_eventsToProcess.begin(), m_eventsToProcess.end(),
                                [&generatedEvent](const XblAchievementsManagerEvent& elem)->bool
                                {
                                    if (generatedEvent.eventType != elem.eventType)
                                    {
                                        return false;
                                    }
                                    if (generatedEvent.progressInfo.achievementId != elem.progressInfo.achievementId)
                                    {
                                        return false;
                                    }
                                    if (generatedEvent.progressInfo.progressState != elem.progressInfo.progressState)
                                    {
                                        return false;
                                    }
                                    auto& cachedProgression = elem.progressInfo.progression;
                                    auto& eventProgression = generatedEvent.progressInfo.progression;
                                    if (eventProgression.timeUnlocked != cachedProgression.timeUnlocked)
                                    {
                                        return false;
                                    }
                                    if (eventProgression.requirementsCount != cachedProgression.requirementsCount)
                                    {
                                        return false;
                                    }
                                    for (uint32_t i = 0; i < eventProgression.requirementsCount; ++i)
                                    {
                                        auto& eventRequirement = eventProgression.requirements[i];
                                        auto& cachedRequirement = cachedProgression.requirements[i];

                                        // If the requirement IDs are different between events for the same achievement, then this is a 
                                        //  unique progress update. If the target progress values are different, then they are also unique,
                                        //  however it likely means that there was a modification to the requirement on the achievement service
                                        //  (which is unlikely to happen with a released game).
                                        if (((String)eventRequirement.id != (String)cachedRequirement.id ||
                                            (String)eventRequirement.targetProgressValue != (String)cachedRequirement.targetProgressValue))
                                        {
                                            return false;
                                        }

                                        // Only recognize this as a unique event if the events current progress is greater than the caches.
                                        if (!ShouldUpdateProgress(eventRequirement.currentProgressValue, cachedRequirement.currentProgressValue))
                                        {
                                            return false;
                                        }
                                    }
                                    return true;
                                }
                            );

                            // Event is unique, so push it onto the list of events to process.
                            if (foundEvent == m_eventsToProcess.end())
                            {
                                m_generatedEvents.push_back(generatedEvent);
                            }
                        }
                    }
                    continue;
                }
                fetchedAchievements.push_back(AchievementsManager::DeepCopyAchievement(achievement));
            }
        }

        if (result.Payload()->HasNext())
        {
            result.Payload()->GetNext(
                achievementsPerFetch,
                AsyncContext<Result<std::shared_ptr<XblAchievementsResult>>>{ async.Queue(),
                [
                    async,
                    achievementsPerFetch,
                    weakThis = std::weak_ptr<AchievementsManagerUser>(shared_from_this()),
                    fetchedAchievements
                ]
            (Result<std::shared_ptr<XblAchievementsResult>> result)
            {
                auto sharedThis{ weakThis.lock() };
                if (sharedThis)
                {
                    sharedThis->HandleAchievementsResults(result, achievementsPerFetch, async, fetchedAchievements);                
                }
            }
            });
        }
        else
        {
            if (!m_isInitialized)
            {
                size_t achievementCount = fetchedAchievements.size();
                m_achievementCache = MakeArray<XblAchievement>(achievementCount);
                for (size_t index = 0; index < achievementCount; ++index)
                {
                    m_achievementCache[index] = fetchedAchievements[index];
                    m_userAchievements[m_achievementCache[index].id] = m_achievementCache + index;
                }
            }
            async.Complete(result.Hresult());
        }
    }
    else
    {
        LOGS_ERROR << "Failed to fetch current state of achievements for user " << m_xuid << " with error code " << result.Hresult();
        async.Complete(result.Hresult());
    }
    return S_OK;
}

//////////////////////////////////////////////////////////

XblAchievement AchievementsManager::CopyAchievementForResult(const XblAchievement& other)
{
    // Most values that are dynamically allocated when creating the 
    //  original achievement struct should not change between frames,
    //  so it is okay if we do shallow copies for those values.
    XblAchievement copy
    {
        other.id,
        other.serviceConfigurationId,
        other.name,
        other.titleAssociations,    // title associations
        other.titleAssociationsCount,
        other.progressState,
        other.progression,  // need to do a deep copy of this
        other.mediaAssets,    // media assets
        other.mediaAssetsCount,
        other.platformsAvailableOn,    // platforms available on
        other.platformsAvailableOnCount,
        other.isSecret,
        other.unlockedDescription,
        other.lockedDescription,
        other.productId,
        other.type,
        other.participationType,
        other.available,
        other.rewards,    // rewards
        other.rewardsCount,
        other.estimatedUnlockTime,
        other.deepLink,
        other.isRevoked
    };

    // Need to do a deep copy of progression so that, if progress values change
    //  while the title is still using a result from a prior frame, we do not
    //  invalidate the memory that the requirement is using. 
    copy.progression.requirements = MakeArray<XblAchievementRequirement>(copy.progression.requirementsCount);
    XblAchievementRequirement* sourceRequirements = other.progression.requirements;
    for (uint64_t i = 0; i < copy.progression.requirementsCount; ++i)
    {
        copy.progression.requirements[i] = XblAchievementRequirement{
            sourceRequirements[i].id,
            Make(sourceRequirements[i].currentProgressValue),
            sourceRequirements[i].targetProgressValue
        };
    }

    return copy;
}

HRESULT AchievementsManager::CleanUpAchievementCopyForResult(XblAchievement& achievement)
{
    for (uint64_t i = 0; i < achievement.progression.requirementsCount; ++i)
    {
        XblAchievementRequirement& requirement = achievement.progression.requirements[i];
        Delete(requirement.currentProgressValue);
    }
    DeleteArray(achievement.progression.requirements, achievement.progression.requirementsCount);

    return S_OK;
}

XblAchievement AchievementsManager::DeepCopyAchievement(_In_ const XblAchievement & other)
{
    XblAchievement copy
    {
        Make(other.id),
        Make(other.serviceConfigurationId),
        Make(other.name),
        nullptr,    // title associations
        other.titleAssociationsCount,
        other.progressState,
        other.progression,  // need to do a deep copy of this
        nullptr,    // media assets
        other.mediaAssetsCount,
        nullptr,    // platforms available on
        other.platformsAvailableOnCount,
        other.isSecret,
        Make(other.unlockedDescription),
        Make(other.lockedDescription),
        Make(other.productId),
        other.type,
        other.participationType,
        other.available,
        nullptr,    // rewards
        other.rewardsCount,
        other.estimatedUnlockTime,
        Make(other.deepLink),
        other.isRevoked
    };

    // titleAssociations
    XblAchievementTitleAssociation* associations = MakeArray<XblAchievementTitleAssociation>(copy.titleAssociationsCount);
    for (uint64_t i = 0; i < copy.titleAssociationsCount; ++i)
    {
        associations[i] = XblAchievementTitleAssociation{
            Make(other.titleAssociations[i].name),
            other.titleAssociations[i].titleId
        };
    }
    copy.titleAssociations = associations;

    // deep copy progression
    copy.progression.requirements = MakeArray<XblAchievementRequirement>(copy.progression.requirementsCount);
    XblAchievementRequirement* sourceRequirements = other.progression.requirements;
    for (uint64_t i = 0; i < copy.progression.requirementsCount; ++i)
    {
        copy.progression.requirements[i] = XblAchievementRequirement{
            Make(sourceRequirements[i].id),
            Make(sourceRequirements[i].currentProgressValue),
            Make(sourceRequirements[i].targetProgressValue)
        };
    }

    auto CloneMediaAsset = [](const XblAchievementMediaAsset& arg)->XblAchievementMediaAsset
    {
        return XblAchievementMediaAsset{
            arg.name ? Make(arg.name) : nullptr,
            arg.mediaAssetType,
            arg.url ? Make(arg.url) : nullptr
        };
    };
    // media assets
    XblAchievementMediaAsset* assets = MakeArray<XblAchievementMediaAsset>(copy.mediaAssetsCount);
    for (uint64_t i = 0; i < copy.mediaAssetsCount; ++i)
    {
        assets[i] = CloneMediaAsset(other.mediaAssets[i]);        
    }
    copy.mediaAssets = assets;

    // platforms
    Vector<String> platforms(other.platformsAvailableOn, other.platformsAvailableOn + other.platformsAvailableOnCount);
    copy.platformsAvailableOn = const_cast<const char**>(MakeArray(platforms));

    // rewards
    XblAchievementReward* rewards = MakeArray<XblAchievementReward>(copy.rewardsCount);
    for (uint64_t i = 0; i < copy.rewardsCount; ++i)
    {
        rewards[i] = XblAchievementReward{
            Make(other.rewards[i].name),
            Make(other.rewards[i].description),
            Make(other.rewards[i].value),
            other.rewards[i].rewardType,
            Make(other.rewards[i].valueType),
            nullptr
        };
        if (other.rewards[i].mediaAsset != nullptr)
        {
            rewards[i].mediaAsset = Make<XblAchievementMediaAsset>(
                CloneMediaAsset(*other.rewards[i].mediaAsset)
            );
        }
    }
    copy.rewards = rewards;
    return copy;
}

XblAchievement* AchievementsManager::DeepCopyAchievements(XblAchievement* cache, size_t size)
{
    XblAchievement* copiedArray = MakeArray<XblAchievement>(cache, size);
    memcpy(copiedArray, cache, sizeof(XblAchievement)*size);
    
    for (size_t index = 0; index < size; ++index)
    {
        // Need to do a deep copy of progression so that, if progress values change
        //  while the title is still using a result from a prior frame, we do not
        //  invalidate the memory that the requirement is using. 
        copiedArray[index].progression.requirements = MakeArray<XblAchievementRequirement>(copiedArray[index].progression.requirementsCount);
        XblAchievementRequirement* sourceRequirements = cache[index].progression.requirements;
        for (uint64_t i = 0; i < copiedArray[index].progression.requirementsCount; ++i)
        {
            copiedArray[index].progression.requirements[i] = XblAchievementRequirement{
                sourceRequirements[i].id,
                Make(sourceRequirements[i].currentProgressValue),
                sourceRequirements[i].targetProgressValue
            };
        }
    }
    return copiedArray;
}

Vector<XblAchievement> AchievementsManager::DeepCopyAchievements(const Vector<XblAchievement>& achievements)
{
    Vector<XblAchievement> achievementsCopy;
    for (const auto& source : achievements)
    {
        achievementsCopy.push_back(CopyAchievementForResult(source));
    }
    return achievementsCopy;
}

HRESULT AchievementsManager::CleanUpDeepCopyAchievement(_In_ XblAchievement& achievement)
{
    Delete(achievement.id);
    Delete(achievement.serviceConfigurationId);
    Delete(achievement.name);
    
    for (uint64_t i = 0; i < achievement.titleAssociationsCount; ++i)
    {
        Delete(achievement.titleAssociations[i].name);
    }
    DeleteArray(achievement.titleAssociations, achievement.titleAssociationsCount);
    
    for (uint64_t i = 0; i < achievement.progression.requirementsCount; ++i)
    {
        XblAchievementRequirement& requirement = achievement.progression.requirements[i];
        Delete(requirement.id);
        Delete(requirement.currentProgressValue);
        Delete(requirement.targetProgressValue);
    }
    DeleteArray(achievement.progression.requirements, achievement.progression.requirementsCount);
        
    for (uint64_t i = 0; i < achievement.mediaAssetsCount; ++i)
    {
        XblAchievementMediaAsset& asset = achievement.mediaAssets[i];
        if (asset.name)
        {
            Delete(asset.name);
        }
        if (asset.url)
        {
            Delete(asset.url);
        }
    }
    DeleteArray(achievement.mediaAssets, achievement.mediaAssetsCount);
    
    DeleteArray(achievement.platformsAvailableOn, achievement.platformsAvailableOnCount);

    Delete(achievement.unlockedDescription);
    Delete(achievement.lockedDescription);
    Delete(achievement.productId);
    
    for (uint64_t i = 0; i < achievement.rewardsCount; ++i)
    {
        XblAchievementReward& reward = achievement.rewards[i];
        Delete(reward.name);
        Delete(reward.description);
        Delete(reward.value);
        Delete(reward.valueType);
        if (reward.mediaAsset != nullptr)
        {
            if (reward.mediaAsset->name)
            {
                Delete(reward.mediaAsset->name);
            }
            if (reward.mediaAsset->url)
            {
                Delete(reward.mediaAsset->url);
            }
            Delete(reward.mediaAsset);
        }
    }
    DeleteArray(achievement.rewards, achievement.rewardsCount);
    
    Delete(achievement.deepLink);
    return S_OK;
}

HRESULT AchievementsManager::AddLocalUser(
    User&& user, 
    TaskQueue && queue
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    auto xuid{ user.Xuid() };
    if (m_localUsers.find(xuid) != m_localUsers.end())
    {
        LOGS_ERROR << "User " << xuid << " already added to AchievementsManager";
        return E_UNEXPECTED;
    }

    auto localUser = MakeShared<AchievementsManagerUser>(std::move(user), queue);

    Result<void> result = localUser->Initialize(AsyncContext<HRESULT>{
        [
            sharedThis{ shared_from_this() },
            weakUser = std::weak_ptr<AchievementsManagerUser>{ localUser->shared_from_this() }
        ]
        (HRESULT hr)
        {
            auto user{ weakUser.lock() };
            if (FAILED(hr))
            {
                LOGS_ERROR << "Failed to initialize local user with ID " << user->Xuid() << " with error code " << hr;
            }

            std::lock_guard<std::mutex> lock{ sharedThis->m_mutex };
            if (user)
            {
                XblAchievementsManagerEvent userAddedEvent{ {}, user->Xuid(), XblAchievementsManagerEventType::LocalUserInitialStateSynced };
                sharedThis->m_pendingEvents.push_back(userAddedEvent);
            }
        }
    });

    if (Succeeded(result))
    {
        m_localUsers[xuid] = localUser;
    }

    return result.Hresult();
}

HRESULT AchievementsManager::RemoveLocalUser(
    _In_ const User & user
) noexcept
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    auto userIter{ m_localUsers.find(user.Xuid()) };
    if (userIter == m_localUsers.end())
    {
        LOGS_ERROR << "User " << user.Xuid() << " was not added to AchievementsManager";
        return E_BOUNDS;
    }

    m_localUsers.erase(userIter);
    m_localUsers.erase(user.Xuid());

    return S_OK;
}

const Vector<XblAchievementsManagerEvent>& AchievementsManager::DoWork() XBL_NOEXCEPT
{
    std::lock_guard<std::mutex> lock{ m_mutex };

    // Clean up the allocations made when doing the copy of the progress entry
    //  for the event.
    for (auto& event : m_publishedEvents)
    {
        AchievementProgressChangeSubscription::CleanUpProgressChangeEntry(event.progressInfo);
    }

    m_publishedEvents = std::move(m_pendingEvents);
    for (auto& pair : m_localUsers)
    {
        auto& user{ pair.second };
        auto achievementEvents = user->ProcessEvents();
        m_publishedEvents.insert(m_publishedEvents.end(), achievementEvents.begin(), achievementEvents.end());
    }

    m_pendingEvents.clear();

    return m_publishedEvents;
}

Result<XblAchievement> AchievementsManager::GetAchievement(
    _In_ uint64_t xuid,
    _In_ const String achievementId
)
{
    if (m_localUsers.find(xuid) == m_localUsers.end())
    {
        char errorMsg[1024];
        SPRINTF(errorMsg, 1024, "User with ID %llu not yet registered with AchievementsManager.", static_cast<unsigned long long>(xuid));
        return { E_BOUNDS, errorMsg};
    }
    if (!m_localUsers[xuid]->IsInitialized())
    {
        char errorMsg[1024];
        SPRINTF(errorMsg, 1024, "User with ID %llu not yet initialized.", static_cast<unsigned long long>(xuid));
        return { E_UNEXPECTED, errorMsg};
    }
    
    return m_localUsers[xuid]->GetAchievement(achievementId);
}

Result<std::pair<XblAchievement*, size_t>> AchievementsManager::GetAchievements(_In_ uint64_t xuid)
{
    if (m_localUsers.find(xuid) == m_localUsers.end())
    {
        char errorMsg[1024];
        SPRINTF(errorMsg, 1024, "User with ID %llu not yet registered with AchievementsManager.", static_cast<unsigned long long>(xuid));
        return { E_BOUNDS, errorMsg };
    }
    if (!m_localUsers[xuid]->IsInitialized())
    {
        char errorMsg[1024];
        SPRINTF(errorMsg, 1024, "User with ID %llu not yet initialized.", static_cast<unsigned long long>(xuid));
        return { E_UNEXPECTED, errorMsg };
    }
    
    return std::pair<XblAchievement*, size_t>({ m_localUsers[xuid]->GetAchievements() , m_localUsers[xuid]->GetAchievementCount() });
}

Result<Vector<XblAchievement>> AchievementsManager::GetAchievements(
    _In_ uint64_t xuid,
    _In_ AchievementsManagerSortFilterSettings requestConfig
)
{
    if (m_localUsers.find(xuid) == m_localUsers.end())
    {
        char errorMsg[1024];
        SPRINTF(errorMsg, 1024, "User with ID %llu not yet registered with AchievementsManager.", static_cast<unsigned long long>(xuid));
        return { E_BOUNDS, errorMsg };
    }
    if (!m_localUsers[xuid]->IsInitialized())
    {
        char errorMsg[1024];
        SPRINTF(errorMsg, 1024, "User with ID %llu not yet initialized.", static_cast<unsigned long long>(xuid));
        return { E_UNEXPECTED, errorMsg };
    }
    
    return m_localUsers[xuid]->GetAchievements(requestConfig);
}

Result<void> AchievementsManager::UpdateAchievement(
    _In_ uint64_t xuid,
    _In_ const String achievementId,
    _In_ uint8_t progress
)
{
    if (m_localUsers.find(xuid) == m_localUsers.end())
    {
        char errorMsg[1024];
        SPRINTF(errorMsg, 1024, "User with ID %llu not yet registered with AchievementsManager.", static_cast<unsigned long long>(xuid));
        return { E_BOUNDS, errorMsg };
    }

    std::shared_ptr<AchievementsManagerUser> localUser = m_localUsers[xuid];
    if (!localUser->IsInitialized())
    {
        char errorMsg[1024];
        SPRINTF(errorMsg, 1024, "User with ID %llu not yet initialized.", static_cast<unsigned long long>(xuid));
        return { E_UNEXPECTED, errorMsg };
    }

    auto isAchievementUpdateable = localUser->CanUpdateAchievement(achievementId, progress);
    if (Failed(isAchievementUpdateable))
    {
        return isAchievementUpdateable;
    }
    
    return localUser->UpdateAchievement(achievementId, progress);
}

bool AchievementsManager::HasUser(_In_ uint64_t xuid) const
{
    return m_localUsers.find(xuid) != m_localUsers.end();
}

bool AchievementsManager::IsUserInitialized(_In_ uint64_t xuid)
{
    return m_localUsers[xuid]->IsInitialized();
}

uint64_t AchievementsManager::GetUserAchievementCount(_In_ uint64_t xuid)
{
    return m_localUsers[xuid]->GetAchievementCount();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_ACHIEVEMENTS_MANAGER_CPP_END



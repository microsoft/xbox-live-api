// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_graph.h"
#include "xbox_live_context_internal.h"
#include "social_manager_user_group.h"
#include "perf_tester.h"

// Max full graph refresh interval - 20 mins in ms
#define GRAPH_REFRESH_INTERVAL_MS (20 * 60 * 1000)

// Presence poll interval - 30 seconds in ms. Presence is only polled if rich presence polling is enabled.
#if XSAPI_UNIT_TESTS
#define PRESENCE_POLL_INTERVAL_MS (1 * 1000)
#else
#define PRESENCE_POLL_INTERVAL_MS (30 * 1000)
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

// Helper class to manage batching, retrying, and throttling of service calls needed by SocialGraph.
// Seperating the state needed to manage the service calls from the actual SocialGraph state for clarity and
// easier state synchronization.
struct ServiceCallManager : public std::enable_shared_from_this<ServiceCallManager>
{
    // Handlers invoked when Presence and Peoplehub Polls complete
    using PresenceResultHandler = std::function<void(Vector<std::shared_ptr<XblPresenceRecord>>&&)>;
    using PeopleHubResultHandler = std::function<void(Vector<XblSocialManagerUser>&&)>;

    ServiceCallManager(
        const User& user,
        const TaskQueue& queue,
        XblSocialManagerExtraDetailLevel peoplehubDetailLevel,
        std::shared_ptr<presence::PresenceService> presenceService,
        std::shared_ptr<PeoplehubService> peoplehubService,
        PresenceResultHandler presenceResultHandler,
        PeopleHubResultHandler peopleHubResultHandler
    ) noexcept;

    ~ServiceCallManager() noexcept;

    // Poll rich presence for a set of users. Result delivered via PresenceResultHandler.
    HRESULT PollPresence(const Vector<uint64_t>& xuids) noexcept;

    // Poll PeopleHub profiles for a set of users. Result delivered via PeoplehubResultHandler.
    HRESULT PollPeopleHub(const Vector<uint64_t>& xuids) noexcept;

    // Get PeopleHub profiles for all followed users. Non-batched, but service failures will be retried
    // automatically. Result delivered via 'handler' arg
    HRESULT PeopleHubGetFollowedUsers(PeopleHubResultHandler handler) const noexcept;

    // Needed to check compatibility between determined detail level and XblPresenceFilter for a social group
    XblSocialManagerExtraDetailLevel GetDetailLevel() const noexcept;

private:
    HRESULT PollPresenceServiceCall(std::unique_lock<std::mutex> lock) noexcept;
    HRESULT PollPeopleHubServiceCall(std::unique_lock<std::mutex> lock) noexcept;

    static constexpr uint32_t c_failureRetryIntervalMs{ 10000 };
    // Adhere to service throttle limits
    static constexpr uint32_t c_presencePollIntervalMs{ 500 };

    TaskQueue const m_queue;
    XblSocialManagerExtraDetailLevel const m_peoplehubDetailLevel;
    uint64_t const m_localUserXuid;

    // Presence polling state
    UnorderedSet<uint64_t> m_usersPendingPresence;
    bool m_presencePollInProgress{ false };
    PresenceResultHandler const m_presenceResultHandler;

    // Peoplehub polling state
    UnorderedSet<uint64_t> m_usersPendingPeoplehub;
    bool m_peoplehubPollInProgress{ false };
    PeopleHubResultHandler const m_peopleHubResultHandler;

    std::shared_ptr<presence::PresenceService> m_presenceService;
    std::shared_ptr<PeoplehubService> m_peoplehubService;
    std::mutex m_mutex;
};

/// -----------------------------------------------------------------------------------------------
/// SocialGraph implementation
/// -----------------------------------------------------------------------------------------------

SocialGraph::SocialGraph(
    _In_ User&& localUser,
    _In_ const TaskQueue& queue,
    _In_ std::shared_ptr<real_time_activity::RealTimeActivityManager> rtaManager
) noexcept : 
    m_user{ MakeShared<User>(std::move(localUser))},
    m_queue{ queue.DeriveWorkerQueue() },
    m_rtaManager{ std::move(rtaManager) }
{
}


HRESULT SocialGraph::Initialize() noexcept
{
    // Maintain legacy RTA activation count.
    m_rtaManager->Activate(*m_user);

    auto copiedUser = m_user->Copy();
    RETURN_HR_IF_FAILED(copiedUser.Hresult());

    m_xblContext = XblContext::Make(copiedUser.ExtractPayload());
    RETURN_HR_IF_FAILED(m_xblContext->Initialize(m_rtaManager));
    m_xblContext->Settings()->SetHttpUserAgent(HttpCallAgent::SocialManager);

    return S_OK;
}

SocialGraph::~SocialGraph()
{
    // Terminate any background work
    m_queue.Terminate(false);

    if (m_socialRelationshipChangedToken)
    {
        m_xblContext->SocialService()->RemoveSocialRelationshipChangedHandler(m_socialRelationshipChangedToken);
    }
    if (m_devicePresenceChangedToken)
    {
        m_xblContext->PresenceService()->RemoveDevicePresenceChangedHandler(m_devicePresenceChangedToken);
    }
    if (m_titlePresenceChangedToken)
    {
        m_xblContext->PresenceService()->RemoveTitlePresenceChangedHandler(m_titlePresenceChangedToken);
    }
    if (m_rtaResyncToken)
    {
        m_rtaManager->RemoveResyncHandler(*m_user, m_rtaResyncToken);
    }

    m_rtaManager->Deactivate(*m_user);
}

Result<std::shared_ptr<SocialGraph>> SocialGraph::Make(
    _In_ User&& user,
    _In_ const XblSocialManagerExtraDetailLevel detailLevel,
    _In_ const TaskQueue& queue,
    _In_ std::shared_ptr<real_time_activity::RealTimeActivityManager> rtaManager
) noexcept
{
    Result<xbox::services::User> userResult = user.Copy();
    if (userResult.Hresult())
    {
        return userResult.Hresult();
    }

    auto graph = std::shared_ptr<SocialGraph>(
        new (Alloc(sizeof(SocialGraph))) SocialGraph{ userResult.ExtractPayload(), queue, rtaManager },
        Deleter<SocialGraph>(),
        Allocator<SocialGraph>()
    );
    auto hr = graph->Initialize();
    if (FAILED(hr))
    {
        return hr;
    }

    std::weak_ptr<SocialGraph> weakGraph{ graph };

    auto peoplehubService = MakeShared<PeoplehubService>(std::move(user), graph->m_xblContext->Settings(), AppConfig::Instance()->TitleId());

    auto presenceService = graph->m_xblContext->PresenceService();

    auto peoplehubResultHandler = [weakGraph](Vector<XblSocialManagerUser>&& profiles)
    {
        if (auto graph{ weakGraph.lock() })
        {
            graph->PeoplehubResultHandler(profiles);
        }
    };

    auto presenceResultHandler = [weakGraph](Vector<std::shared_ptr<XblPresenceRecord>>&& records)
    {
        if (auto graph{ weakGraph.lock() })
        {
            graph->PresenceResultHandler(records);
        }
    };

    graph->m_serviceCallManager = MakeShared<ServiceCallManager>(
        user,
        queue,
        detailLevel,
        presenceService,
        std::move(peoplehubService),
        presenceResultHandler,
        peoplehubResultHandler
    );

    graph->m_getSocialGraphTask = PeriodicTask::MakeAndRun(queue, GRAPH_REFRESH_INTERVAL_MS, [weakGraph]
    {
        if (auto graph{ weakGraph.lock() })
        {
            graph->m_serviceCallManager->PeopleHubGetFollowedUsers([weakGraph](Vector<XblSocialManagerUser>&& profiles)
            {
                if (auto graph{ weakGraph.lock() })
                {
                    // Update observer counts based on updated follower list
                    std::unique_lock<std::recursive_mutex> lock{ graph->m_mutex };
                    Vector<uint64_t> previouslyFollowedXuids, followedXuids;
                    for (auto& pair : graph->m_profiles)
                    {
                        if (pair.second->isFollowedByCaller)
                        {
                            previouslyFollowedXuids.push_back(pair.first);
                        }
                    }
                    std::transform(profiles.begin(), profiles.end(), std::back_inserter(followedXuids), [](const XblSocialManagerUser& profile)
                    {
                        return profile.xboxUserId;
                    });
                    lock.unlock();

                    // Don't repoll profiles since we just called PeopleHub
                    graph->TrackUsers(followedXuids, PeoplehubPollMode::Never);
                    graph->StopTrackingUsers(previouslyFollowedXuids);

                    // Generate graph updates
                    graph->PeoplehubResultHandler(profiles);

                    // Build initial graph on background thread since we don't care about generating events during initialization
                    if (!graph->m_initialized)
                    {
                        lock.lock();
                        Vector<XblSocialManagerEvent> events;
                        Vector<std::shared_ptr<XblSocialManagerUser>> affectedUsers;
                        graph->ApplyGraphUpdates(events, affectedUsers);
                        graph->m_initialized = true;
                    }
                }
            });

            std::unique_lock<std::recursive_mutex> lock{ graph->m_mutex };
            Vector<uint64_t> nonFollowedXuids;
            for (auto& pair : graph->m_profiles)
            {
                if (!pair.second->isFollowedByCaller)
                {
                    nonFollowedXuids.push_back(pair.first);
                }
            }
            lock.unlock();

            if (!nonFollowedXuids.empty())
            {
                graph->m_serviceCallManager->PollPeopleHub(nonFollowedXuids);
            }
        }
    });

    graph->m_socialRelationshipChangedToken = graph->m_xblContext->SocialService()->AddSocialRelationshipChangedHandler(
        [weakGraph](const XblSocialRelationshipChangeEventArgs& args)
        {
            if (auto graph{ weakGraph.lock() })
            {
                graph->SocialRelationshipChangedHandler(args);
            }
        });

    // Presence change handlers can in theory be optimized to avoid a service call when presence ends,
    // but it requires remembering a lot more state. Handling all presence changes notifications uniformly
    // by just repolling presence simplifies the logic dramatically.

    graph->m_devicePresenceChangedToken = presenceService->AddDevicePresenceChangedHandler(
        [weakGraph](uint64_t xuid, XblPresenceDeviceType deviceType, bool isUserLoggedOnDevice)
        {
            UNREFERENCED_PARAMETER(deviceType);
            UNREFERENCED_PARAMETER(isUserLoggedOnDevice);

            if (auto graph{ weakGraph.lock() })
            {
                graph->m_serviceCallManager->PollPresence({ xuid });
            }
        });

    graph->m_titlePresenceChangedToken = presenceService->AddTitlePresenceChangedHandler(
        [weakGraph](uint64_t xuid, uint32_t titleId, XblPresenceTitleState state)
        {
            UNREFERENCED_PARAMETER(titleId);
            UNREFERENCED_PARAMETER(state);

            if (auto graph{ weakGraph.lock() })
            {
                graph->m_serviceCallManager->PollPresence({ xuid });
            }
        });

    graph->m_rtaResyncToken = rtaManager->AddResyncHandler(user, [weakGraph]
        {
            if (auto graph{ weakGraph.lock() })
            {
                graph->m_getSocialGraphTask->ScheduleImmediately();
            }
        });

    return graph;
}

std::shared_ptr<User> SocialGraph::LocalUser() const noexcept
{
    return m_user;
}

void SocialGraph::DoWork(
    _Inout_ Vector<XblSocialManagerEvent>& events,
    _Inout_ Vector<std::shared_ptr<XblSocialManagerUser>>& affectedUsers
) noexcept
{
    PERF_START();
    // For performance reasons, don't wait for the mutex if a background thread holds it
    std::unique_lock<std::recursive_mutex> lock{ m_mutex, std::defer_lock };
    if (lock.try_lock() && m_initialized)
    {
        // Raise the LocalUserAdded Event in the first DoWork call after the intial graph has loaded
        if (!m_localUserAdded)
        {
            events.push_back(XblSocialManagerEvent{ m_user->Handle(), XblSocialManagerEventType::LocalUserAdded });
            m_localUserAdded = true;
        }

        // Apply graph updates
        ApplyGraphUpdates(events, affectedUsers);

        // Notify groups of graph changes
        for (auto& pair : m_groups)
        {
            // If group isn't initialized, schedule initialization to background queue.
            switch (pair.second)
            {
            case GroupInitializationStage::Pending:
            {
                pair.second = GroupInitializationStage::Scheduled;

                m_queue.RunWork([this, sharedThis{ shared_from_this() }, group{ pair.first }]
                {
                    std::unique_lock<std::recursive_mutex> lock{ m_mutex };
                    group->Initialize(m_profiles);
                    m_groups[group] = GroupInitializationStage::Complete;
                });

                break;
            }
            case GroupInitializationStage::Complete:
            {
                pair.first->DoWork(events);
                break;
            }
            case GroupInitializationStage::Scheduled:
            default:
            {
                break;
            }
            }
        }
    }
    PERF_STOP();
}

void SocialGraph::RegisterGroup(std::shared_ptr<XblSocialManagerUserGroup> group) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };

    // If this is a new group or an existing group that has already completed initialization, set initialization stage
    // so that it gets initialized during DoWork.
    auto iter{ m_groups.find(group) };
    if (iter == m_groups.end() || iter->second == GroupInitializationStage::Complete)
    {
        m_groups[group] = GroupInitializationStage::Pending;

        // Check if the filter is one that relies on title history but TitleHistoryLevel is not set
        if ((group->presenceFilter == XblPresenceFilter::TitleOffline || 
             group->presenceFilter == XblPresenceFilter::TitleOnlineOutsideTitle || 
             group->presenceFilter == XblPresenceFilter::AllTitle) && 
            (m_serviceCallManager->GetDetailLevel() & XblSocialManagerExtraDetailLevel::TitleHistoryLevel) != XblSocialManagerExtraDetailLevel::TitleHistoryLevel)
        {
            LOGS_DEBUG << "TitleOffline, TitleOnlineOutsideTitle, and AllTitle filters require XblSocialManagerExtraDetailLevel::TitleHistoryLevel to be set for this user";
        }
    }
}

void SocialGraph::UnregisterGroup(std::shared_ptr<XblSocialManagerUserGroup> group) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };
    m_groups.erase(group);
}

void SocialGraph::TrackUsers(
    _In_ const Vector<uint64_t>& xuids
) noexcept
{
    // Only poll PeopleHub for profiles not already in the graph
    TrackUsers(xuids, PeoplehubPollMode::IfNew);
}

void SocialGraph::TrackUsers(
    _In_ const Vector<uint64_t>& xuids,
    _In_ PeoplehubPollMode refreshMode
) noexcept
{
    std::unique_lock<std::recursive_mutex> lock{ m_mutex };

    Vector<uint64_t> refreshXuids;
    for (auto xuid : xuids)
    {
        auto iter = m_trackedUsers.find(xuid);
        if (iter == m_trackedUsers.end())
        {
            // Add new graph entry
            m_trackedUsers.emplace(xuid, TrackedUser{ xuid, m_xblContext->PresenceService() });
            if (refreshMode == PeoplehubPollMode::Always || refreshMode == PeoplehubPollMode::IfNew)
            {
                refreshXuids.emplace_back(xuid);
            }
        }
        else
        {
            ++iter->second.refCount;
            if (refreshMode == PeoplehubPollMode::Always)
            {
                refreshXuids.emplace_back(xuid);
            }
        }
    }

    lock.unlock();
    if (!refreshXuids.empty())
    {
        m_serviceCallManager->PollPeopleHub(refreshXuids);
    }
}

void SocialGraph::StopTrackingUsers(
    _In_ const Vector<uint64_t>& xuids
) noexcept
{
    std::lock_guard<std::recursive_mutex> lock{ m_mutex };

    for (auto xuid : xuids)
    {
        auto iter{ m_trackedUsers.find(xuid) };
        if (--iter->second.refCount == 0)
        {
            m_trackedUsers.erase(iter);
            m_pendingUpdates[xuid] = { ProfileChanges::None, nullptr };
        }
    }
}

void SocialGraph::SetRichPresencePolling(bool enabled) noexcept
{
    if (enabled != m_presencePollingEnabled)
    {
        m_presencePollingEnabled = enabled;
        if (m_presencePollingEnabled)
        {
            assert(!m_getPresenceForGraphTask);

            m_getPresenceForGraphTask = PeriodicTask::MakeAndRun(m_queue, PRESENCE_POLL_INTERVAL_MS, 
                [
                    weakGraph = std::weak_ptr<SocialGraph>{ shared_from_this() },
                    this
                ]
            {
                if (auto graph{weakGraph.lock()})
                {
                    std::unique_lock<std::recursive_mutex> lock{ m_mutex };
                    Vector<uint64_t> xuids;
                    std::transform(m_trackedUsers.begin(), m_trackedUsers.end(), std::back_inserter(xuids), [](const auto& pair)
                    {
                        return pair.first;
                    });

                    lock.unlock();
                    m_serviceCallManager->PollPresence(xuids);
                }
            });
        }
        else
        {
            assert(m_getPresenceForGraphTask);
            m_getPresenceForGraphTask.reset();
        }
    }
}

void SocialGraph::PeoplehubResultHandler(
    const Vector<XblSocialManagerUser>& users
) noexcept
{
    PERF_START();
    std::unique_lock<std::recursive_mutex> lock{ m_mutex };

    for (auto& profile : users)
    {
        auto iter{ m_profiles.find(profile.xboxUserId) };
        if (iter == m_profiles.end())
        {
            m_pendingUpdates[profile.xboxUserId] = { ProfileChanges::None, MakeShared<XblSocialManagerUser>(profile) };
        }
        else if (auto changes = CompareProfiles(*iter->second, profile))
        {
            m_pendingUpdates[profile.xboxUserId] = { changes, MakeShared<XblSocialManagerUser>(profile) };
        }
    }
    PERF_STOP();
}

void SocialGraph::PresenceResultHandler(
    const Vector<std::shared_ptr<XblPresenceRecord>>& presenceRecords
) noexcept
{
    PERF_START();
    std::unique_lock<std::recursive_mutex> lock{ m_mutex };
    for (auto& record : presenceRecords)
    {
        // When comparing with existing profile, first check if there is a pending update
        std::shared_ptr<XblSocialManagerUser> compareProfile{ nullptr };

        auto updatesIter{ m_pendingUpdates.find(record->Xuid()) };
        if (updatesIter != m_pendingUpdates.end())
        {
            compareProfile = updatesIter->second.second;
        }
        if (!compareProfile)
        {
            auto profilesIter{ m_profiles.find(record->Xuid()) };
            if (profilesIter != m_profiles.end())
            {
                compareProfile = profilesIter->second;
            }
        }

        // If there is no entry to compare, ignore. The profile (including presence) will be updated
        // when the associated Peoplehub call completes
        if (compareProfile)
        {
            auto smRecord{ ConvertPresenceRecord(record) };
            if (memcmp(&compareProfile->presenceRecord, &smRecord, sizeof(XblSocialManagerPresenceRecord)))
            {
                auto updatedProfile = MakeShared<XblSocialManagerUser>(*compareProfile);
                memcpy(&updatedProfile->presenceRecord, &smRecord, sizeof(XblSocialManagerPresenceRecord));

                m_pendingUpdates[updatedProfile->xboxUserId] = { ProfileChanges::PresenceChanged, updatedProfile };
            }
        }
    }
    PERF_STOP();
}

void SocialGraph::SocialRelationshipChangedHandler(
    const XblSocialRelationshipChangeEventArgs& args
) noexcept
{
    // Depending on the type of notification and whether or not the user is still being tracked
    // in our graph, we will need to refresh the profiles for a subset of the affected xuids.
    Vector<uint64_t> affectedXuids(args.xboxUserIds, args.xboxUserIds + args.xboxUserIdsCount);
    Vector<uint64_t> refreshXuids;

    switch (args.socialNotification)
    {
    case XblSocialNotificationType::Added:
    {
        TrackUsers(affectedXuids, PeoplehubPollMode::Always);
        break;
    }
    case XblSocialNotificationType::Changed:
    {
        refreshXuids = std::move(affectedXuids);
        break;
    }
    case XblSocialNotificationType::Removed:
    {
        StopTrackingUsers(affectedXuids);

        // If the users are still in the graph, refresh their profile
        std::unique_lock<std::recursive_mutex> lock{ m_mutex };
        for (auto xuid : affectedXuids)
        {
            if(m_trackedUsers.find(xuid) != m_trackedUsers.end())
            {
                refreshXuids.push_back(xuid);
            }
        }
        break;
    }
    default:
    {
        LOGS_DEBUG << "Unknown social notification received and ignored.";
    }
    }

    if (!refreshXuids.empty())
    {
        m_serviceCallManager->PollPeopleHub(refreshXuids);
    }
}

void SocialGraph::AddOrUpdateEvent(
    XblSocialManagerEventType type,
    const std::shared_ptr<XblSocialManagerUser>& affectedUser,
    Vector<XblSocialManagerEvent>& events,
    Vector<std::shared_ptr<XblSocialManagerUser>>& affectedUsers
) noexcept
{
    PERF_START();

    // Update affected users set
    affectedUsers.push_back(affectedUser);

    // Try to update an existing event first
    for (auto& event : events)
    {
        if (event.eventType == type)
        {
            uint8_t affectedUserIndex{ 0 };
            for (; affectedUserIndex < std::extent<decltype(event.usersAffected)>::value && event.usersAffected[affectedUserIndex]; ++affectedUserIndex);
            if (affectedUserIndex < std::extent<decltype(event.usersAffected)>::value)
            {
                event.usersAffected[affectedUserIndex] = affectedUser.get();
                return;
            }
        }
    }

    // If we couldn't update an existing event, add a new one
    events.emplace_back();
    auto& newEvent{ events.back() };
    newEvent.eventType = type;
    newEvent.user = m_user->Handle();
    newEvent.usersAffected[0] = affectedUser.get();

    PERF_STOP();
}

void SocialGraph::ApplyGraphUpdates(
    _Inout_ Vector<XblSocialManagerEvent>& events,
    _Inout_ Vector<std::shared_ptr<XblSocialManagerUser>>& affectedUsers
) noexcept
{
    PERF_START();

    // Apply updates. After initialization, apply at most MAX_GRAPH_UPDATES_PER_FRAME
    size_t updatesApplied = 0;
    size_t updateLimit{ m_initialized ? MAX_GRAPH_UPDATES_PER_FRAME : 1000u };

    for (auto updateIter = m_pendingUpdates.begin(); updateIter != m_pendingUpdates.end() && updatesApplied < updateLimit; updateIter = m_pendingUpdates.erase(updateIter), ++updatesApplied)
    {
        auto& xuid{ updateIter->first };
        auto trackedUserIter{ m_trackedUsers.find(xuid) };
        auto profileIter{ m_profiles.find(xuid) };
        auto profileChanges{ updateIter->second.first };
        auto& updatedProfile{ updateIter->second.second };

        // If we are no longer tracking the user remove the profile and add event
        if (trackedUserIter == m_trackedUsers.end() && profileIter != m_profiles.end())
        {
            AddOrUpdateEvent(XblSocialManagerEventType::UsersRemovedFromSocialGraph, profileIter->second, events, affectedUsers);
            m_profiles.erase(profileIter);
        }
        else if (updatedProfile) // This could be null in cases where the user is untracked/tracked in the same DoWork cycle
        {
            // If this is a new profile, generate only a user added event. Otherwise generate depending on the profileChanges
            if (profileIter == m_profiles.end())
            {
                AddOrUpdateEvent(XblSocialManagerEventType::UsersAddedToSocialGraph, updatedProfile, events, affectedUsers);
                m_profiles.insert({ xuid, updatedProfile });
            }
            else
            {
                profileIter->second = updatedProfile;
                if (profileChanges & ProfileChanges::PresenceChanged)
                {
                    AddOrUpdateEvent(XblSocialManagerEventType::PresenceChanged, updatedProfile, events, affectedUsers);
                }
                if (profileChanges & ProfileChanges::RelationshipChanged)
                {
                    AddOrUpdateEvent(XblSocialManagerEventType::SocialRelationshipsChanged, updatedProfile, events, affectedUsers);
                }
                if (profileChanges & ProfileChanges::ProfileChanged)
                {
                    AddOrUpdateEvent(XblSocialManagerEventType::ProfilesChanged, updatedProfile, events, affectedUsers);
                }
            }
        }
    }
    PERF_STOP();
}

ProfileChanges SocialGraph::CompareProfiles(
    const XblSocialManagerUser& old,
    const XblSocialManagerUser& updated
) noexcept
{
    auto changes{ ProfileChanges::None };

    // ProfileChanges::RelationshipChanged indicates favorite/following/followed changed
    if (old.isFollowedByCaller != updated.isFollowedByCaller ||
        old.isFollowingUser != updated.isFollowingUser ||
        old.isFavorite != updated.isFavorite
    )
    {
        changes |= ProfileChanges::RelationshipChanged;
    }
    // ProfileChanges::PresenceChange indicates a change in the presence record
    if (memcmp(&old.presenceRecord, &updated.presenceRecord, sizeof(XblSocialManagerPresenceRecord)))
    {
        changes |= ProfileChanges::PresenceChanged;
    }
    // ProfileChanges::ProfileChanged indicates any other change
    if (utils::str_icmp(old.gamerscore, updated.gamerscore) != 0 ||
        old.titleHistory.hasUserPlayed != updated.titleHistory.hasUserPlayed ||
        old.titleHistory.lastTimeUserPlayed != updated.titleHistory.lastTimeUserPlayed ||
        utils::str_icmp(old.displayPicUrlRaw, updated.displayPicUrlRaw) != 0 ||
        old.useAvatar != updated.useAvatar ||
        utils::str_icmp(old.gamertag, updated.gamertag) != 0 ||
        utils::str_icmp(old.modernGamertag, updated.modernGamertag) != 0 ||
        utils::str_icmp(old.modernGamertagSuffix, updated.modernGamertagSuffix) != 0 ||
        utils::str_icmp(old.uniqueModernGamertag, updated.uniqueModernGamertag) != 0 ||
        utils::str_icmp(old.displayName, updated.displayName) != 0 ||
        utils::str_icmp(old.realName, updated.realName) != 0 ||
        utils::str_icmp(old.preferredColor.primaryColor, updated.preferredColor.primaryColor) != 0 ||
        utils::str_icmp(old.preferredColor.secondaryColor, updated.preferredColor.secondaryColor) != 0 ||
        utils::str_icmp(old.preferredColor.tertiaryColor, updated.preferredColor.tertiaryColor) != 0
    )
    {
        changes |= ProfileChanges::ProfileChanged;
    }
    return changes;
}

XblSocialManagerPresenceRecord SocialGraph::ConvertPresenceRecord(
    std::shared_ptr<XblPresenceRecord> presenceRecord
) noexcept
{
    XblSocialManagerPresenceRecord smPresenceRecord{};
    smPresenceRecord.userState = presenceRecord->UserState();

    for (auto& deviceRecord : presenceRecord->DeviceRecords())
    {
        for (uint32_t i = 0; i < deviceRecord.titleRecordsCount && smPresenceRecord.presenceTitleRecordCount < XBL_NUM_PRESENCE_RECORDS; ++i)
        {
            auto& smTitleRecord{ smPresenceRecord.presenceTitleRecords[smPresenceRecord.presenceTitleRecordCount++] };
            auto& newTitleRecord{ deviceRecord.titleRecords[i] };

            smTitleRecord.titleId = newTitleRecord.titleId;
            smTitleRecord.isTitleActive = newTitleRecord.titleActive;
            utils::strcpy(smTitleRecord.titleName, sizeof(smTitleRecord.titleName), newTitleRecord.titleName);
            utils::strcpy(smTitleRecord.presenceText, sizeof(smTitleRecord.presenceText), newTitleRecord.richPresenceString);
            smTitleRecord.isBroadcasting = newTitleRecord.broadcastRecord != nullptr;
            smTitleRecord.deviceType = deviceRecord.deviceType;
        }

        if (smPresenceRecord.presenceTitleRecordCount == XBL_NUM_PRESENCE_RECORDS)
        {
            break;
        }
    }
    return smPresenceRecord;
}

/// -----------------------------------------------------------------------------------------------
/// TrackedUser implementation
/// -----------------------------------------------------------------------------------------------

TrackedUser::TrackedUser(
    uint64_t _xuid,
    std::shared_ptr<presence::PresenceService> _presenceService
) noexcept
    : xuid{ _xuid },
    presenceService{ std::move(_presenceService) }
{
    PERF_START();
    HRESULT hr = presenceService->TrackUsers({ xuid });
    assert(SUCCEEDED(hr));
    UNREFERENCED_PARAMETER(hr);
    PERF_STOP();
}

TrackedUser::TrackedUser(const TrackedUser& other) noexcept
    : TrackedUser{ other.xuid, other.presenceService }
{
}

TrackedUser::~TrackedUser() noexcept
{
    PERF_START();
    assert(presenceService);
    presenceService->StopTrackingUsers({ xuid });
    PERF_STOP();
}

/// -----------------------------------------------------------------------------------------------
/// ServiceCallManager implementation
/// -----------------------------------------------------------------------------------------------

ServiceCallManager::ServiceCallManager(
    const User& user,
    const TaskQueue& queue,
    XblSocialManagerExtraDetailLevel peoplehubDetailLevel,
    std::shared_ptr<presence::PresenceService> presenceService,
    std::shared_ptr<PeoplehubService> peoplehubService,
    PresenceResultHandler presenceResultHandler,
    PeopleHubResultHandler peoplehubResultHandler
) noexcept
    : m_queue{ queue.DeriveWorkerQueue() },
    m_peoplehubDetailLevel{ peoplehubDetailLevel },
    m_localUserXuid{ user.Xuid() },
    m_presenceResultHandler{ std::move(presenceResultHandler) },
    m_peopleHubResultHandler{ std::move(peoplehubResultHandler) },
    m_presenceService{ std::move(presenceService) },
    m_peoplehubService{ std::move(peoplehubService) }
{
}

ServiceCallManager::~ServiceCallManager() noexcept
{
    m_queue.Terminate(false);
}

HRESULT ServiceCallManager::PollPresence(const Vector<uint64_t>& xuids) noexcept
{
    std::unique_lock<std::mutex> lock{ m_mutex };

    m_usersPendingPresence.insert(xuids.begin(), xuids.end());
    if (!m_presencePollInProgress)
    {
        return PollPresenceServiceCall(std::move(lock));
    }
    return S_OK;
}

HRESULT ServiceCallManager::PollPeopleHub(const Vector<uint64_t>& xuids) noexcept
{
    std::unique_lock<std::mutex> lock{ m_mutex };

    m_usersPendingPeoplehub.insert(xuids.begin(), xuids.end());
    if (!m_peoplehubPollInProgress)
    {
        return PollPeopleHubServiceCall(std::move(lock));
    }
    return S_OK;
}

HRESULT ServiceCallManager::PeopleHubGetFollowedUsers(PeopleHubResultHandler handler) const noexcept
{
    return m_peoplehubService->GetSocialGraph(m_localUserXuid, m_peoplehubDetailLevel, { m_queue,
        [
            weakThis = std::weak_ptr<ServiceCallManager const>{ shared_from_this() },
            this,
            handler{ std::move(handler) }
        ]
    (Result<Vector<XblSocialManagerUser>> result)
    {
        if (Failed(result))
        {
            m_queue.RunWork([weakThis, this, handler]
            {
                if (auto sharedThis{ weakThis.lock() })
                {
                    PeopleHubGetFollowedUsers(handler);
                }
            }, c_failureRetryIntervalMs);
        }
        else
        {
            handler(result.ExtractPayload());
        }
    }
    });
}

HRESULT ServiceCallManager::PollPresenceServiceCall(std::unique_lock<std::mutex> lock) noexcept
{
    assert(lock.owns_lock());
    if (m_usersPendingPresence.empty())
    {
        return S_OK;
    }

    XblPresenceQueryFilters filters{};
    filters.detailLevel = XblPresenceDetailLevel::All;

    Vector<uint64_t> pollXuids{ m_usersPendingPresence.begin(), m_usersPendingPresence.end() };

    auto hr = m_presenceService->GetBatchPresence(
        presence::UserBatchRequest{ pollXuids.data(), pollXuids.size(), &filters }, { m_queue,
        [
            weakThis = std::weak_ptr<ServiceCallManager>{ shared_from_this() },
            this,
            pollXuids
        ]
    (Result<Vector<std::shared_ptr<XblPresenceRecord>>> result)
    {
        if (auto sharedThis{ weakThis.lock() })
        {
            uint32_t interval{ c_presencePollIntervalMs };

            std::unique_lock<std::mutex> lock{ sharedThis->m_mutex };
            if (Failed(result))
            {
                // Ensure failed xuids are retried. Increase poll interval for failures
                m_usersPendingPresence.insert(pollXuids.begin(), pollXuids.end());
                interval = c_failureRetryIntervalMs;
            }
            else
            {
                m_presenceResultHandler(result.ExtractPayload());
            }

            // Schedule next poll if xuids are still pending
            if (!m_usersPendingPresence.empty())
            {
                m_queue.RunWork([weakThis, this]
                {
                    if (auto sharedThis{ weakThis.lock() })
                    {
                        PollPresenceServiceCall(std::unique_lock<std::mutex>{ m_mutex });
                    }
                }, interval);
            }
            else
            {
                m_presencePollInProgress = false;
            }
        }
    }
    });

    m_presencePollInProgress = true;
    m_usersPendingPresence.clear();

    return hr;
}

HRESULT ServiceCallManager::PollPeopleHubServiceCall(std::unique_lock<std::mutex> lock) noexcept
{
    assert(lock.owns_lock());
    if (m_usersPendingPeoplehub.empty())
    {
        return S_OK;
    }

    Vector<uint64_t> pollXuids{ m_usersPendingPeoplehub.begin(), m_usersPendingPeoplehub.end() };

    auto hr = m_peoplehubService->GetSocialUsers(m_localUserXuid, m_peoplehubDetailLevel, pollXuids, { m_queue,
        [
            weakThis = std::weak_ptr<ServiceCallManager>{ shared_from_this() },
            this,
            pollXuids
        ]
    (Result<Vector<XblSocialManagerUser>> result)
    {
        if (auto sharedThis{ weakThis.lock() })
        {
            uint32_t interval{ 0 };

            std::unique_lock<std::mutex> lock{ sharedThis->m_mutex };
            if (Failed(result))
            {
                // Ensure failed xuids are retried. Increase poll interval for failures
                m_usersPendingPeoplehub.insert(pollXuids.begin(), pollXuids.end());
                interval = c_failureRetryIntervalMs;
            }
            else
            {
                m_peopleHubResultHandler(result.ExtractPayload());
            }

            // Schedule next poll if xuids are still pending
            if (!m_usersPendingPeoplehub.empty())
            {
                m_queue.RunWork([weakThis, this]
                {
                    if (auto sharedThis{ weakThis.lock() })
                    {
                        PollPeopleHubServiceCall(std::unique_lock<std::mutex>{ m_mutex });
                    }
                }, interval);
            }
            else
            {
                m_peoplehubPollInProgress = false;
            }
        }
    }
    });

    m_peoplehubPollInProgress = true;
    m_usersPendingPeoplehub.clear();

    return hr;
}

XblSocialManagerExtraDetailLevel ServiceCallManager::GetDetailLevel() const noexcept
{
    return m_peoplehubDetailLevel;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END
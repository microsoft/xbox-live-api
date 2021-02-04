// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once

#include "presence_internal.h"
#include "social_internal.h"
#include "peoplehub_service.h"
#include "real_time_activity_manager.h"

#ifdef max
#undef max
#endif

// Throttle the maximum number of updates the SocialGraph will process each call to DoWork
#define MAX_GRAPH_UPDATES_PER_FRAME 5

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

// Enum describing how a profile has changed
enum ProfileChanges : uint32_t
{
    None = 0x0,
    ProfileChanged = 0x1,
    PresenceChanged = 0x2,
    RelationshipChanged = 0x4
};

DEFINE_ENUM_FLAG_OPERATORS(ProfileChanges);

// Metadata about a user tracked by SocialGraph. Maintains RTA tracking and counts references within the SocialGraph
struct TrackedUser
{
    TrackedUser(
        uint64_t xuid,
        std::shared_ptr<presence::PresenceService> presenceService
    ) noexcept;
    TrackedUser(const TrackedUser& other) noexcept;
    TrackedUser& operator=(TrackedUser) = delete;
    ~TrackedUser() noexcept;

    uint32_t refCount{ 1 };
    uint64_t xuid;
    std::shared_ptr<presence::PresenceService> presenceService;
};

// Tracks profile and presence changes for other XboxLiveUsers. Automatically tracks users followed by the 
// local user, but additional remote users can be added explicitly. 
class SocialGraph : public std::enable_shared_from_this<SocialGraph>
{
public:
    static Result<std::shared_ptr<SocialGraph>> Make(
        _In_ User&& localUser,
        _In_ const XblSocialManagerExtraDetailLevel detailLevel,
        _In_ const TaskQueue& queue,
        _In_ std::shared_ptr<real_time_activity::RealTimeActivityManager> rtaManager
    ) noexcept;

    ~SocialGraph();

    // LocalUser that the SocialGraph is for
    std::shared_ptr<User> LocalUser() const noexcept;

    // Patches profile and presence updates that have happened since the last call to DoWork into the graph.
    // Appends XblSocialManagerEvents describing the updates to 'events' list. Appends all affected users to the list of affected users.
    // This is used to maintain their lifetime since the events reference them.
    void DoWork(
        _Inout_ Vector<XblSocialManagerEvent>& events,
        _Inout_ Vector<std::shared_ptr<XblSocialManagerUser>>& affectedUsers
    ) noexcept;

    // Registers/Unregisters an XblSocialManagerUserGroup backed by this graph. Registered groups
    // will be initialized and then notified of graph changes each call to DoWork
    void RegisterGroup(std::shared_ptr<XblSocialManagerUserGroup> group) noexcept;
    void UnregisterGroup(std::shared_ptr<XblSocialManagerUserGroup> group) noexcept;

    // Track a list of users. If they are not currently tracked, they will be added to the graph.
    void TrackUsers(
        _In_ const Vector<uint64_t>& xuids
    ) noexcept;

    // Stop tracking a list of users that were previously added via TrackUsers.
    void StopTrackingUsers(
        _In_ const Vector<uint64_t>& xuids
    ) noexcept;

    // Enable/Disbale rich presence polling. If set to true, creates a Periodic Task to refresh presence and
    // runs it immediately.
    void SetRichPresencePolling(bool enabled) noexcept;

    HRESULT Initialize() noexcept;
private:
    SocialGraph(
        _In_ User&& localUser,
        _In_ const TaskQueue& queue,
        _In_ std::shared_ptr<real_time_activity::RealTimeActivityManager> rtaManager
    ) noexcept;

    // Generate Graph Updates based on the result of a PeopleHub service call
    void PeoplehubResultHandler(
        const Vector<XblSocialManagerUser>& users
    ) noexcept;

    // Generate Graph Updates based on the result of a Presence service call
    void PresenceResultHandler(
        const Vector<std::shared_ptr<XblPresenceRecord>>& presenceRecords
    ) noexcept;

    // RTA handler
    void SocialRelationshipChangedHandler(const XblSocialRelationshipChangeEventArgs& args) noexcept;

    // Enum describing how to update a tracked User's profile
    enum PeoplehubPollMode { Never, IfNew, Always };
    void TrackUsers(
        _In_ const Vector<uint64_t>& xuids,
        _In_ PeoplehubPollMode refreshMode
    ) noexcept;

    // Helper that aggregates events based on graph updates
    inline void AddOrUpdateEvent(
        XblSocialManagerEventType type,
        const std::shared_ptr<XblSocialManagerUser>& affectedUser,
        Vector<XblSocialManagerEvent>& events,
        Vector<std::shared_ptr<XblSocialManagerUser>>& affectedUsers
    ) noexcept;

    // Applies all pending updates to local graph. Updates events and affected users
    void ApplyGraphUpdates(
        _Inout_ Vector<XblSocialManagerEvent>& events,
        _Inout_ Vector<std::shared_ptr<XblSocialManagerUser>>& affectedUsers
    ) noexcept;

    static ProfileChanges CompareProfiles(
        const XblSocialManagerUser& old,
        const XblSocialManagerUser& updated
    ) noexcept;

    static XblSocialManagerPresenceRecord ConvertPresenceRecord(
        std::shared_ptr<XblPresenceRecord> presenceRecord
    ) noexcept;

    std::shared_ptr<User> m_user;
    TaskQueue const m_queue;

    // Graph state
    UnorderedMap<uint64_t, std::shared_ptr<XblSocialManagerUser>> m_profiles;
    UnorderedMap<uint64_t, TrackedUser> m_trackedUsers;
    UnorderedMap<uint64_t, std::pair<ProfileChanges, std::shared_ptr<XblSocialManagerUser>>> m_pendingUpdates;

    // Groups. Initialization stage indicates whether or not a group has been initialized yet
    enum GroupInitializationStage{ Pending, Scheduled, Complete };
    Map<std::shared_ptr<XblSocialManagerUserGroup>, GroupInitializationStage> m_groups;

    bool m_presencePollingEnabled{ false };
    bool m_localUserAdded{ false };
    bool m_initialized{ false };

    std::shared_ptr<real_time_activity::RealTimeActivityManager> m_rtaManager;
    std::shared_ptr<XblContext> m_xblContext;
    std::shared_ptr<struct ServiceCallManager> m_serviceCallManager;

    // Handler tokens
    XblFunctionContext m_socialRelationshipChangedToken{ 0 };
    XblFunctionContext m_devicePresenceChangedToken{ 0 };
    XblFunctionContext m_titlePresenceChangedToken{ 0 };
    XblFunctionContext m_rtaResyncToken{ 0 };

    // Background PeriodicTasks
    std::shared_ptr<PeriodicTask> m_getPresenceForGraphTask;
    std::shared_ptr<PeriodicTask> m_getSocialGraphTask;

    mutable std::recursive_mutex m_mutex;
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END
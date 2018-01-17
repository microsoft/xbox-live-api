// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/social_manager.h"
#include "xsapi/system.h"
#include "xsapi/social.h"
#if XSAPI_U
#include "ppltasks_extra_unix.h"
#else
#include "ppltasks_extra.h"
#endif
#include "system_internal.h"
#include "user_context.h"
#include "xsapi/mem.h"
#include "perf_tester.h"
#include "call_buffer_timer.h"

typedef unsigned char byte;

namespace xbox { namespace services { namespace system { 
    class xbox_live_mutex;
}}}

namespace xbox { namespace services { namespace social { namespace manager {

/// <summary>
/// internal only
/// </summary>
enum class change_list_enum
{
    no_change = 0x0,
    profile_change = 0x1,
    presence_change = 0x2,
    social_relationship_change = 0x4,
    change = 0x8
};

enum class internal_social_event_type
{
    unknown,
    users_changed,
    users_removed,
    presence_changed,
    device_presence_changed,
    title_presence_changed,
    profiles_changed,
    social_relationships_changed,
    users_added
};

enum class social_graph_state
{
    normal,
    diff,
    event_processing,
    refresh
};

struct user_group_status_change
{
    xsapi_internal_vector<string_t> addGroup;
    xsapi_internal_vector<uint64_t> removeGroup;
};

inline change_list_enum operator|(change_list_enum lhs, change_list_enum rhs)
{
    return static_cast<change_list_enum>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

inline change_list_enum operator&(change_list_enum lhs, change_list_enum rhs)
{
    return static_cast<change_list_enum>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

class internal_social_event
{
public:
    internal_social_event() : m_socialEventType(internal_social_event_type::unknown) {}
    internal_social_event(_In_ internal_social_event_type eventType, _In_ xsapi_internal_vector<xbox_social_user> usersAffected);
    internal_social_event(_In_ internal_social_event_type eventType, _In_ xsapi_internal_vector<social_manager_presence_record> presenceRecords);
    internal_social_event(_In_ internal_social_event_type eventType, _In_ xbox::services::presence::device_presence_change_event_args devicePresenceArgs);
    internal_social_event(_In_ internal_social_event_type eventType, _In_ xbox::services::presence::title_presence_change_event_args titlePresenceArgs);
    internal_social_event(_In_ internal_social_event_type eventType, _In_ xsapi_internal_vector<uint64_t> userList);
    internal_social_event(
        _In_ internal_social_event_type socialEventType,
        _In_ xbox_live_result<void> errorInfo,
        _In_ xsapi_internal_vector<xsapi_internal_string> userList
        );

    internal_social_event(
        _In_ internal_social_event_type eventType,
        _In_ xsapi_internal_vector<xsapi_internal_string> userAddList,
        _In_ pplx::task_completion_event<xbox_live_result<void>> tce
        );

    internal_social_event(
        _In_ internal_social_event_type eventType,
        _In_ xsapi_internal_vector<xsapi_internal_string> userAddList
        );

    const call_buffer_timer_completion_context& completion_context() const;
    void set_completion_context(_In_ const call_buffer_timer_completion_context& compleitionContext);
    const xsapi_internal_vector<xbox_social_user>& users_affected() const;
    const xsapi_internal_vector<uint64_t>& users_to_remove() const;
    const xsapi_internal_vector<social_manager_presence_record>& presence_records() const;
    const xbox::services::presence::device_presence_change_event_args& device_presence_args() const;
    const xbox::services::presence::title_presence_change_event_args& title_presence_args() const;
    const xsapi_internal_vector<xsapi_internal_string>& users_affected_as_string_vec() const;
    const pplx::task_completion_event<xbox_live_result<void>>& tce() const;
    const xbox_live_result<void>& error() const;
    internal_social_event_type event_type() const;

private:
    internal_social_event_type m_socialEventType;
    call_buffer_timer_completion_context m_completionContext;
    xsapi_internal_vector<social_manager_presence_record> m_presenceRecords;
    xsapi_internal_vector<xbox_social_user> m_usersAffected;
    xsapi_internal_vector<xsapi_internal_string> m_usersAffectedAsStringVec;
    xsapi_internal_vector<uint64_t> m_userList;
    pplx::task_completion_event<xbox_live_result<void>> m_tce;
    xbox::services::presence::device_presence_change_event_args m_devicePresenceArgs;
    xbox::services::presence::title_presence_change_event_args m_titlePresenceArgs;
    xbox_live_result<void> m_error;
};

struct xbox_social_user_context
{
    uint32_t refCount;
    xbox_social_user* socialUser;
};

struct xbox_social_user_subscriptions
{
    std::shared_ptr<xbox::services::presence::device_presence_change_subscription> devicePresenceChangeSubscription;
    std::shared_ptr<xbox::services::presence::title_presence_change_subscription> titlePresenceChangeSubscription;
};

struct change_struct
{
    const xsapi_internal_unordered_map<uint64_t, xbox_social_user_context>* socialUsers;
};

class internal_event_queue
{
public:
    template<typename T, typename U>
    void push(_In_ internal_social_event_type socialEventType, _In_ const std::vector<T, U> userList, _In_ const call_buffer_timer_completion_context& completionContext = call_buffer_timer_completion_context())
    {
        std::lock_guard<std::mutex> lock(m_eventMutex.get());
        std::lock_guard<std::mutex> priorityLock(m_eventPriorityMutex.get());
        auto numGroupsofUsers = userList.size() / MAX_USERS_AFFECTED_PER_EVENT + 1;
        for (uint32_t i = 0; i < numGroupsofUsers; ++i)
        {
            auto endLoc = __min((i + 1) * MAX_USERS_AFFECTED_PER_EVENT, userList.size());
            std::vector<T, U> usersAffected(userList.begin() + i * MAX_USERS_AFFECTED_PER_EVENT, userList.begin() + endLoc);
            auto evt = internal_social_event(socialEventType, usersAffected);
            if (i == 0 && !completionContext.isNull)
            {
                evt.set_completion_context(completionContext);
            }
            m_eventQueue.push_back(evt);
        }
    }

    void push(_In_ const internal_social_event& socialEvent)
    {
        std::lock_guard<std::mutex> lock(m_eventMutex.get());
        std::lock_guard<std::mutex> priorityLock(m_eventPriorityMutex.get());
        m_eventQueue.push_back(socialEvent);
    }

    internal_social_event pop()
    {
        std::lock_guard<std::mutex> lock(m_eventMutex.get());
        std::lock_guard<std::mutex> priorityLock(m_eventPriorityMutex.get());
        internal_social_event evt = m_eventQueue.front();
        m_eventQueue.pop_front();
        return evt;
    }

    size_t size()
    {
        std::lock_guard<std::mutex> lock(m_eventMutex.get());
        std::lock_guard<std::mutex> priorityLock(m_eventPriorityMutex.get());
        return m_eventQueue.size();
    }

    bool empty(_In_ bool isPriority = false)
    {
        if (!isPriority)
        {
            std::lock_guard<std::mutex> lock(m_eventMutex.get());
        }
        std::lock_guard<std::mutex> priorityLock(m_eventPriorityMutex.get());
        return m_eventQueue.empty();
    }

private:
    static const uint32_t MAX_USERS_AFFECTED_PER_EVENT = 10;

    bool m_useLock;
    xsapi_internal_dequeue<internal_social_event> m_eventQueue;
    xbox::services::system::xbox_live_mutex m_eventMutex;
    xbox::services::system::xbox_live_mutex m_eventPriorityMutex;
};

struct user_buffer
{
    user_buffer() : buffer(nullptr) {}

    byte* buffer;
    std::queue<byte*> freeData;
    xsapi_internal_unordered_map<uint64_t, xbox_social_user_context> socialUserGraph;
    internal_event_queue socialUserEventQueue;
};

class user_buffers_holder
{
public:
    user_buffers_holder();
    ~user_buffers_holder();

    void initialize(_In_ const std::vector<xbox_social_user>& users);
    void swap();

    user_buffer& user_buffer_a();
    user_buffer& user_buffer_b();

    user_buffer* active_buffer();
    user_buffer* inactive_buffer();

    void add_event(
        _In_ const internal_social_event& internalSocialEvent
        );

    void add_users_to_buffer(_In_ const std::vector<xbox_social_user>& users, _Inout_ user_buffer& userBufferInactive, _In_ size_t finalSize = 0);

    void remove_users_from_buffer(_In_ const std::vector<uint64_t>& users, _Inout_ user_buffer& userBufferInactive);

    static void initialize_users_in_map(_Inout_ user_buffer& userBuffer, _In_ size_t numUsers, _In_ size_t bufferOffset);

protected:
    void initialize_buffer(_Inout_ user_buffer& userBuffer, _In_ const std::vector<xbox_social_user>& users, _In_ size_t freeSpaceRequired = 0);

    void buffer_init(_Inout_ user_buffer& userBuffer, _In_ const std::vector<xbox_social_user>& users, _In_ size_t freeSpaceRequired);

    static void add_users_impl(_In_ const xsapi_internal_vector<xbox_social_user>& users, _Inout_ user_buffer& userBufferActive, _Inout_ user_buffer& userBufferInactive);

    static void remove_users_impl(_In_ const xsapi_internal_vector<xbox_social_user>& users, _Inout_ user_buffer& userBufferActive, _Inout_ user_buffer& userBufferInactive);

    byte* buffer_alloc(
        _In_ size_t numUsers,
        _Inout_ size_t& allocatedSize,
        _In_ size_t freeSpaceRequired
        );

    static const uint32_t EXTRA_USER_FREE_SPACE;

    user_buffer* m_activeBuffer;
    user_buffer* m_inactiveBuffer;
    user_buffer m_userBufferA;
    user_buffer m_userBufferB;
};

enum class event_state
{
    read,
    ready_to_read,
    clear
};

class event_queue
{
public:
    event_queue();
    event_queue(_In_ const xbox_live_user_t& user_t);
    void push(
        _In_ const internal_social_event& socialEvent,
        _In_ xbox_live_user_t user,
        _In_ social_event_type type,
        _In_ xbox_live_result<void> error = xbox_live_result<void>()
        );

    bool empty();
    void clear();
    size_t size()
    {
        return m_socialEventList.size();
    }
    const std::vector<social_event>& social_event_list();

private:
    event_state m_eventState;
    uint32_t m_lastKnownSize;
    xbox_live_user_t m_user;
    std::vector<xbox::services::social::manager::social_event> m_socialEventList;
    xbox::services::system::xbox_live_mutex m_eventGraphMutex;

    static social_event_type convert_internal_social_event_type_to_social_event_type(_In_ internal_social_event_type socialEventType);
};

class peoplehub_service
{
public:
    peoplehub_service() {};

    peoplehub_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> httpCallSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
        );

    pplx::task<xbox_live_result<std::vector<xbox::services::social::manager::xbox_social_user>>> get_social_graph(
        _In_ const string_t& callerXboxUserId,
        _In_ social_manager_extra_detail_level decorations
        );

    pplx::task<xbox_live_result<std::vector<xbox::services::social::manager::xbox_social_user>>> get_social_graph(
        _In_ const string_t& callerXboxUserId,
        _In_ social_manager_extra_detail_level decorations,
        _In_ const std::vector<string_t> xboxLiveUsers
        );

    pplx::task<xbox_live_result<std::vector<xbox::services::social::manager::xbox_social_user>>> get_suggested_friends(
        _In_ const string_t& xboxUserId,
        _In_ social_manager_extra_detail_level decorations
        );

private:
    pplx::task<xbox_live_result<std::vector<xbox::services::social::manager::xbox_social_user>>> get_social_graph(
        _In_ const string_t& callerXboxUserId,
        _In_ social_manager_extra_detail_level decorations,
        _In_ const string_t& relationshipType,
        _In_ const std::vector<string_t> xboxLiveUsers,
        _In_ bool isBatch
        );

    string_t social_graph_subpath(
        _In_ const string_t& xboxUserId,
        _In_ social_manager_extra_detail_level decorations,
        _In_ const string_t& relationshipType,
        _In_ const std::vector<string_t> xboxLiveUsers,
        _In_ bool isBatch
        ) const;

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_httpCallSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config> m_appConfig;
};

class social_graph_snapshot
{
public:
    social_graph_snapshot(
        _In_ xsapi_internal_unordered_map<string_t, xbox_social_user_context> snapshot
        );

    const xsapi_internal_unordered_map<string_t, xbox_social_user_context>& snapshot();
private:
    xsapi_internal_unordered_map<string_t, xbox_social_user_context> m_socialUsers;
};


class social_graph : public std::enable_shared_from_this<social_graph>
{
public:
    social_graph(
        _In_ xbox_live_user_t user,
        _In_ social_manager_extra_detail_level socialManagerExtraDetailLevel,
        _In_ std::function<void()> graphDestructionCompleteCallback
        );

    virtual ~social_graph();

    pplx::task<xbox_live_result<void>> virtual initialize();

    uint32_t title_id();

    change_struct do_work(_Inout_ std::vector<social_event>& socialEvents);

    void add_users(_In_ const std::vector<string_t>& users, _In_ const pplx::task_completion_event<xbox_live_result<void>>& tce);

    void remove_users(_In_ const std::vector<uint64_t>& users);

    bool is_initialized();

    bool are_events_empty();

    void clear_debug_counters();

    void print_debug_info();
    
    void enable_rich_presence_polling(_In_ bool shouldEnablePolling);

    const xsapi_internal_unordered_map<uint64_t, xbox_social_user_context>* active_buffer_social_graph();

protected:
    static const std::chrono::minutes REFRESH_TIME_MIN;

    static const uint32_t NUM_EVENTS_PER_FRAME;

    static const std::chrono::seconds TIME_PER_CALL_SEC;

    void setup_rta();

    void setup_rta_subscriptions(
        _In_ bool shouldReinitialize = false
        );

    void refresh_graph();

    bool process_events();

    void initialize_social_buffers(_In_ const std::vector<xbox_social_user>& socialUsers);

    void social_graph_refresh_callback();

    void presence_refresh_callback();

    bool do_event_work();

    void presence_timer_callback(
        _In_ const std::vector<string_t>& users
        );

    pplx::task<xbox_live_result<std::vector<xbox_social_user>>> social_graph_timer_callback(
        _In_ const xsapi_internal_vector<xsapi_internal_string>& users,
        _In_ const call_buffer_timer_completion_context& completionContext
        );

    void update_graph(
        _In_ std::vector<std::shared_ptr<xbox_social_user>> userList
        );

    void handle_title_presence_change(
        _In_ xbox::services::presence::title_presence_change_event_args titlePresenceChanged
        );

    void handle_device_presence_change(
        _In_ xbox::services::presence::device_presence_change_event_args devicePresenceChanged
        );

    void handle_social_relationship_change(
        _In_ xbox::services::social::social_relationship_change_event_args socialRelationshipChanged
        );

    void handle_rta_subscription_error(
        _In_ xbox::services::real_time_activity::real_time_activity_subscription_error_event_args& rtaErrorEventArgs
        );

    void handle_rta_connection_state_change(
        _In_ xbox::services::real_time_activity::real_time_activity_connection_state rtaState
        );

    void perform_diff(_In_ const xsapi_internal_unordered_map<uint64_t, xbox_social_user>& xboxSocialUsers);

    void set_state(_In_ social_graph_state socialGraphState);

    void process_cached_events();

    void apply_event(
        _In_ const internal_social_event& evt,
        _In_ bool applyToEventQueue
        );

    void setup_device_and_presence_subscriptions(
        _In_ const std::vector<uint64_t>& users
        );

    void setup_device_and_presence_subscriptions_helper(
        _In_ const std::vector<uint64_t>& users
        );

    void unsubscribe_users(
        _In_ const std::vector<uint64_t>& users
        );

    void _Trigger_rta_connection_state_change_event(_In_ xbox::services::real_time_activity::real_time_activity_connection_state state);

    void apply_users_change_event(_In_ const internal_social_event& socialEvent, _In_ user_buffer* inactiveBuffer, _In_ bool isFreshEvent);

    void apply_users_removed_event(_In_ const internal_social_event& socialEvent, _In_ user_buffer* inactiveBuffer, _Inout_ social_event_type& eventType, _In_ bool isFreshEvent);

    void apply_users_added_event(_In_ const internal_social_event& socialEvent, _In_ user_buffer* inactiveBuffer, _In_ bool isFreshEvent);

    void apply_device_presence_changed_event(_In_ const internal_social_event& socialEvent, _In_ user_buffer* inactiveBuffer, _In_ bool isFreshEvent, _Inout_ social_event_type& eventType);

    void apply_presence_changed_event(_In_ const internal_social_event& socialEvent, _In_ user_buffer* inactiveBuffer, _In_ bool isFreshEvent);

    void refresh_graph_helper(std::vector<uint64_t>& userRefreshList);


    bool m_isInitialized;
    bool m_wasDisconnected;
    bool m_isPollingRichPresence;

    //rta function contexts
    function_context m_devicePresenceContext;
    function_context m_titlePresenceContext;
    function_context m_socialRelationshipContext;
    function_context m_resyncContext;
    function_context m_subscriptionErrorContext;
    function_context m_rtaStateChangeContext;

    uint32_t m_numEventsThisFrame;
    uint32_t m_userAddedContext;

    social_manager_extra_detail_level m_detailLevel;
    social_graph_state m_socialGraphState;

    xbox_live_user_t m_user;
    std::unique_ptr<bool> m_shouldCancel;
    std::shared_ptr<xbox_live_context_impl> m_xboxLiveContextImpl;
    std::shared_ptr<call_buffer_timer> m_presenceRefreshTimer;
    std::shared_ptr<call_buffer_timer> m_presencePollingTimer;
    std::shared_ptr<call_buffer_timer> m_socialGraphRefreshTimer;
    std::shared_ptr<call_buffer_timer> m_resyncRefreshTimer;
    std::shared_ptr<xbox::services::social::social_relationship_change_subscription> m_socialRelationshipChangeSubscription;
    peoplehub_service m_peoplehubService;
    std::function<void()> m_graphDestructionCompleteCallback;
    std::function<void(_In_ xbox::services::real_time_activity::real_time_activity_connection_state state)> m_stateRTAFunction;
    xsapi_internal_unordered_map<uint64_t, xbox_social_user_subscriptions> m_socialUserSubscriptions;
    std::recursive_mutex m_socialGraphMutex;
    std::recursive_mutex m_socialGraphPriorityMutex;
    std::recursive_mutex m_socialGraphStateMutex;
    xbox::services::perf_tester m_perfTester;
    event_queue m_socialEventQueue;
    internal_event_queue m_internalEventQueue;
    user_buffers_holder m_userBuffer;
};

}}}}

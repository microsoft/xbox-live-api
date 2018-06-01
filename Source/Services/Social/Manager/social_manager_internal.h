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
#include "presence_internal.h"

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

enum class unprocessed_social_event_type
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
    xsapi_internal_vector<xsapi_internal_string> addGroup;
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

struct xbox_removal_struct
{
    uint64_t xuidNum;
    xbox_user_id_container xuidContainer;
};

class unprocessed_social_event
{
public:
    unprocessed_social_event() : m_socialEventType(unprocessed_social_event_type::unknown) {}
    unprocessed_social_event(_In_ unprocessed_social_event_type eventType, _In_ xsapi_internal_vector<xbox_social_user> usersAffected);
    unprocessed_social_event(_In_ unprocessed_social_event_type eventType, _In_ xsapi_internal_vector<social_manager_presence_record> presenceRecords);
    unprocessed_social_event(_In_ unprocessed_social_event_type eventType, _In_ std::shared_ptr<xbox::services::presence::device_presence_change_event_args_internal> devicePresenceArgs);
    unprocessed_social_event(_In_ unprocessed_social_event_type eventType, _In_ std::shared_ptr<xbox::services::presence::title_presence_change_event_args_internal> titlePresenceArgs);
    unprocessed_social_event(_In_ unprocessed_social_event_type eventType, _In_ xsapi_internal_vector<uint64_t> userList);
    unprocessed_social_event(
        _In_ unprocessed_social_event_type socialEventType,
        _In_ xbox_live_result<void> errorInfo,
        _In_ xsapi_internal_vector<xsapi_internal_string> userList
        );

    unprocessed_social_event(
        _In_ unprocessed_social_event_type eventType,
        _In_ xsapi_internal_vector<xsapi_internal_string> userAddList,
        _In_ xbox_live_callback<xbox_live_result<void>> callback
        );

    unprocessed_social_event(
        _In_ unprocessed_social_event_type eventType,
        _In_ xsapi_internal_vector<xsapi_internal_string> userAddList
        );

    std::shared_ptr<call_buffer_timer_completion_context> completion_context() const;
    void set_completion_context(_In_ std::shared_ptr<call_buffer_timer_completion_context> compleitionContext);
    const xsapi_internal_vector<xbox_social_user>& users_affected() const;
    const xsapi_internal_vector<uint64_t>& users_to_remove() const;
    const xsapi_internal_vector<social_manager_presence_record>& presence_records() const;
    const std::shared_ptr<xbox::services::presence::device_presence_change_event_args_internal> device_presence_args() const;
    const std::shared_ptr<xbox::services::presence::title_presence_change_event_args_internal> title_presence_args() const;
    const xsapi_internal_vector<xsapi_internal_string>& users_affected_as_string_vec() const;
    xbox_live_callback<xbox_live_result<void>> callback;
    const xbox_live_result<void>& error() const;
    unprocessed_social_event_type event_type() const;

private:
    unprocessed_social_event_type m_socialEventType;
    std::shared_ptr<call_buffer_timer_completion_context> m_completionContext;
    xsapi_internal_vector<social_manager_presence_record> m_presenceRecords;
    xsapi_internal_vector<xbox_social_user> m_usersAffected;
    xsapi_internal_vector<xsapi_internal_string> m_usersAffectedAsStringVec;
    xsapi_internal_vector<uint64_t> m_userList;
    std::shared_ptr<xbox::services::presence::device_presence_change_event_args_internal> m_devicePresenceArgs;
    std::shared_ptr<xbox::services::presence::title_presence_change_event_args_internal> m_titlePresenceArgs;
    xbox_live_result<void> m_error;
};

class social_event_internal
{
public:
    social_event_internal(
        _In_ xbox_live_user_t user,
        _In_ social_event_type eventType,
        _In_ xsapi_internal_vector<xbox_user_id_container> usersAffected,
        _In_ std::shared_ptr<social_event_args> socialEventArgs = nullptr,
        _In_ std::error_code errCode = xbox_live_error_code::no_error,
        _In_ xsapi_internal_string errMessage = xsapi_internal_string()
        );

    xbox_live_user_t user() const;

    social_event_type event_type() const;

    const xsapi_internal_vector<xbox_user_id_container>& users_affected() const;

    const std::shared_ptr<social_event_args>& event_args() const;

    const std::error_code& err() const;

    const xsapi_internal_string& err_message() const;

private:
    social_event_type m_eventType;
    std::error_code m_errCode;
    xbox_live_user_t m_user;
    std::shared_ptr<social_event_args> m_eventArgs;
    xsapi_internal_vector<xbox_user_id_container> m_usersAffected;
    xsapi_internal_string m_errMessage;
};

class social_user_group_loaded_event_args_internal : public social_event_args
{
public:
    std::shared_ptr<xbox_social_user_group_internal> social_user_group() const;

    social_user_group_loaded_event_args_internal(_In_ std::shared_ptr<xbox_social_user_group_internal> socialUserGroup);

private:
    std::shared_ptr<xbox_social_user_group_internal> m_socialUserGroup;
};

struct xbox_social_user_context
{
    uint32_t refCount;
    xbox_social_user* socialUser;
};

struct xbox_social_user_subscriptions
{
    std::shared_ptr<xbox::services::presence::device_presence_change_subscription_internal> devicePresenceChangeSubscription;
    std::shared_ptr<xbox::services::presence::title_presence_change_subscription_internal> titlePresenceChangeSubscription;
};

struct change_struct
{
    const xsapi_internal_unordered_map<uint64_t, xbox_social_user_context>* socialUsers;
};

class unprocessed_event_queue
{
public:
    template<typename T, typename U>
    void push(_In_ unprocessed_social_event_type socialEventType, _In_ const std::vector<T, U> userList, _In_ std::shared_ptr<call_buffer_timer_completion_context> completionContext = nullptr)
    {
        std::lock_guard<std::mutex> lock(m_eventMutex.get());
        std::lock_guard<std::mutex> priorityLock(m_eventPriorityMutex.get());
        auto numGroupsofUsers = userList.size() / MAX_USERS_AFFECTED_PER_EVENT + 1;
        for (uint32_t i = 0; i < numGroupsofUsers; ++i)
        {
            auto endLoc = __min((i + 1) * MAX_USERS_AFFECTED_PER_EVENT, userList.size());
            std::vector<T, U> usersAffected(userList.begin() + i * MAX_USERS_AFFECTED_PER_EVENT, userList.begin() + endLoc);
            auto evt = unprocessed_social_event(socialEventType, usersAffected);
            if (i == 0 && completionContext != nullptr)
            {
                evt.set_completion_context(completionContext);
            }
            m_eventQueue.push_back(evt);
        }
    }

    void push(_In_ const unprocessed_social_event& socialEvent)
    {
        std::lock_guard<std::mutex> lock(m_eventMutex.get());
        std::lock_guard<std::mutex> priorityLock(m_eventPriorityMutex.get());
        m_eventQueue.push_back(socialEvent);
    }

    unprocessed_social_event pop()
    {
        std::lock_guard<std::mutex> lock(m_eventMutex.get());
        std::lock_guard<std::mutex> priorityLock(m_eventPriorityMutex.get());
        unprocessed_social_event evt = m_eventQueue.front();
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
    xsapi_internal_dequeue<unprocessed_social_event> m_eventQueue;
    xbox::services::system::xbox_live_mutex m_eventMutex;
    xbox::services::system::xbox_live_mutex m_eventPriorityMutex;
};

struct user_buffer
{
    user_buffer() : buffer(nullptr) {}

    byte* buffer;
    xsapi_internal_queue<byte*> freeData;
    xsapi_internal_unordered_map<uint64_t, xbox_social_user_context> socialUserGraph;
    unprocessed_event_queue socialUserEventQueue;
};

class user_buffers_holder
{
public:
    user_buffers_holder();
    ~user_buffers_holder();

    void initialize(_In_ const xsapi_internal_vector<xbox_social_user>& users);
    void swap();

    user_buffer& user_buffer_a();
    user_buffer& user_buffer_b();

    user_buffer* active_buffer();
    user_buffer* inactive_buffer();

    void add_event(
        _In_ const unprocessed_social_event& internalSocialEvent
        );

    void add_users_to_buffer(_In_ const xsapi_internal_vector<xbox_social_user>& users, _Inout_ user_buffer& userBufferInactive, _In_ size_t finalSize = 0);

    void remove_users_from_buffer(_In_ const xsapi_internal_vector<uint64_t>& users, _Inout_ user_buffer& userBufferInactive);

    static void initialize_users_in_map(_Inout_ user_buffer& userBuffer, _In_ size_t numUsers, _In_ size_t bufferOffset);

protected:
    void initialize_buffer(_Inout_ user_buffer& userBuffer, _In_ const xsapi_internal_vector<xbox_social_user>& users, _In_ size_t freeSpaceRequired = 0);

    void buffer_init(_Inout_ user_buffer& userBuffer, _In_ const xsapi_internal_vector<xbox_social_user>& users, _In_ size_t freeSpaceRequired);

    static void add_users_impl(_In_ const xsapi_internal_vector<xbox_social_user>& users, _Inout_ user_buffer& userBufferActive, _Inout_ user_buffer& userBufferInactive);

    static void remove_users_impl(_In_ const xsapi_internal_vector<xbox_social_user>& users, _Inout_ user_buffer& userBufferActive, _Inout_ user_buffer& userBufferInactive);

    byte* buffer_alloc(
        _In_ size_t numUsers,
        _Out_ size_t& allocatedSize,
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
        _In_ const unprocessed_social_event& socialEvent,
        _In_ xbox_live_user_t user,
        _In_ social_event_type type,
        _In_ xbox_live_result<void> error = xbox_live_result<void>()
        );

    bool empty();
    void clear();
    const xsapi_internal_vector<std::shared_ptr<social_event_internal>>& social_event_list();

private:
    event_state m_eventState;
    uint32_t m_lastKnownSize;
    xbox_live_user_t m_user;
    xsapi_internal_vector<std::shared_ptr<social_event_internal>> m_socialEventList;
    xbox::services::system::xbox_live_mutex m_eventGraphMutex;
};

class peoplehub_service
{
public:
    peoplehub_service() {};

    peoplehub_service(
        _In_ std::shared_ptr<xbox::services::user_context> userContext,
        _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> httpCallSettings,
        _In_ std::shared_ptr<xbox::services::xbox_live_app_config_internal> appConfig
        );

    void get_social_graph(
        _In_ const xsapi_internal_string& callerXboxUserId,
        _In_ social_manager_extra_detail_level decorations,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<xbox_social_user>>> callback
        );

    void get_social_graph(
        _In_ const xsapi_internal_string& callerXboxUserId,
        _In_ social_manager_extra_detail_level decorations,
        _In_ const xsapi_internal_vector<xsapi_internal_string> xboxLiveUsers,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<xbox_social_user>>> callback
        );

    void get_suggested_friends(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ social_manager_extra_detail_level decorations,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<xbox_social_user>>> callback
        );

private:
    void get_social_graph(
        _In_ const xsapi_internal_string& callerXboxUserId,
        _In_ social_manager_extra_detail_level decorations,
        _In_ const xsapi_internal_string& relationshipType,
        _In_ const xsapi_internal_vector<xsapi_internal_string> xboxLiveUsers,
        _In_ bool isBatch,
        _In_opt_ async_queue_handle_t queue,
        _In_ xbox_live_callback<xbox_live_result<xsapi_internal_vector<xbox_social_user>>> callback
        );

    xsapi_internal_string social_graph_subpath(
        _In_ const xsapi_internal_string& xboxUserId,
        _In_ social_manager_extra_detail_level decorations,
        _In_ const xsapi_internal_string& relationshipType,
        _In_ const xsapi_internal_vector<xsapi_internal_string> xboxLiveUsers,
        _In_ bool isBatch
        ) const;

    std::shared_ptr<xbox::services::user_context> m_userContext;
    std::shared_ptr<xbox::services::xbox_live_context_settings> m_httpCallSettings;
    std::shared_ptr<xbox::services::xbox_live_app_config_internal> m_appConfig;
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
        _In_ xbox_live_callback<void> graphDestructionCompleteCallback,
        _In_ async_queue_handle_t backgroundAsyncQueue = nullptr
        );

    virtual ~social_graph();

    virtual void initialize(xbox_live_callback<xbox_live_result<void>> callback);

    uint32_t title_id();

    change_struct do_work(_Inout_ xsapi_internal_vector<std::shared_ptr<social_event_internal>>& socialEvents);

    void add_users(
        _In_ const xsapi_internal_vector<xsapi_internal_string>& users, 
        _In_ xbox_live_callback<xbox_live_result<void>> callback
        );

    void remove_users(_In_ const xsapi_internal_vector<uint64_t>& users);

    bool is_initialized();

    bool are_events_empty();

    void clear_debug_counters();

    void print_debug_info();

    void enable_rich_presence_polling(_In_ bool shouldEnablePolling);

    const xsapi_internal_unordered_map<uint64_t, xbox_social_user_context>* active_buffer_social_graph();

    void set_background_async_queue(async_queue_handle_t queue);

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

    void initialize_social_buffers(_In_ const xsapi_internal_vector<xbox_social_user>& socialUsers);

    void schedule_social_graph_refresh();

    void schedule_presence_refresh();

    void schedule_event_work();
    bool do_event_work();

    void presence_timer_callback(
        _In_ const xsapi_internal_vector<xsapi_internal_string>& users
        );

    void social_graph_timer_callback(
        _In_ const xsapi_internal_vector<xsapi_internal_string>& users,
        _In_ std::shared_ptr<call_buffer_timer_completion_context> completionContext
        );

    void update_graph(
        _In_ xsapi_internal_vector<std::shared_ptr<xbox_social_user>> userList
        );

    void handle_title_presence_change(
        _In_ std::shared_ptr<xbox::services::presence::title_presence_change_event_args_internal> titlePresenceChanged
        );

    void handle_device_presence_change(
        _In_ std::shared_ptr<xbox::services::presence::device_presence_change_event_args_internal> devicePresenceChanged
        );

    void handle_social_relationship_change(
        _In_ std::shared_ptr<xbox::services::social::social_relationship_change_event_args_internal> socialRelationshipChanged
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
        _In_ const unprocessed_social_event& evt,
        _In_ bool applyToEventQueue
        );

    void setup_device_and_presence_subscriptions(
        _In_ const xsapi_internal_vector<uint64_t>& users
        );

    void setup_device_and_presence_subscriptions_helper(
        _In_ const xsapi_internal_vector<uint64_t>& users
        );

    void unsubscribe_users(
        _In_ const xsapi_internal_vector<uint64_t>& users
        );

    void _Trigger_rta_connection_state_change_event(_In_ xbox::services::real_time_activity::real_time_activity_connection_state state);

    void apply_users_change_event(_In_ const unprocessed_social_event& socialEvent, _In_ user_buffer* inactiveBuffer, _In_ bool isFreshEvent);

    void apply_users_removed_event(_In_ const unprocessed_social_event& socialEvent, _In_ user_buffer* inactiveBuffer, _Inout_ social_event_type& eventType, _In_ bool isFreshEvent);

    void apply_users_added_event(_In_ const unprocessed_social_event& socialEvent, _In_ user_buffer* inactiveBuffer, _In_ bool isFreshEvent);

    void apply_device_presence_changed_event(_In_ const unprocessed_social_event& socialEvent, _In_ user_buffer* inactiveBuffer, _In_ bool isFreshEvent, _Inout_ social_event_type& eventType);

    void apply_presence_changed_event(_In_ const unprocessed_social_event& socialEvent, _In_ user_buffer* inactiveBuffer, _In_ bool isFreshEvent);

    void refresh_graph_helper(xsapi_internal_vector<uint64_t>& userRefreshList);

    template <typename T>
    void invoke_callback(xbox_live_callback<T> callback, T result)
    {
        struct invoke_callback_context
        {
            T result;
            xbox_live_callback<T> callback;
        };

        auto context = xsapi_allocate_shared<invoke_callback_context>();
        context->result = result;
        context->callback = callback;

        AsyncBlock* async = new (xbox::services::system::xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
        async->queue = m_backgroundAsyncQueue;
        async->callback = [](AsyncBlock* asyncBlock)
        {
            xsapi_memory::mem_free(asyncBlock);
        };

        BeginAsync(async, utils::store_shared_ptr(context), nullptr, __FUNCTION__,
            [](AsyncOp op, const AsyncProviderData* data)
        {
            if (op == AsyncOp_DoWork)
            {
                auto context = utils::get_shared_ptr<invoke_callback_context>(data->context);
                context->callback(context->result);
                CompleteAsync(data->async, S_OK, 0);
                return E_PENDING;
            }
            return S_OK;
        });
        ScheduleAsync(async, 0);
    }

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
    std::atomic<bool> m_shouldCancel;
    std::shared_ptr<xbox_live_context_impl> m_xboxLiveContextImpl;
    std::shared_ptr<call_buffer_timer> m_presenceRefreshTimer;
    std::shared_ptr<call_buffer_timer> m_presencePollingTimer;
    std::shared_ptr<call_buffer_timer> m_socialGraphRefreshTimer;
    std::shared_ptr<call_buffer_timer> m_resyncRefreshTimer;
    std::shared_ptr<xbox::services::social::social_relationship_change_subscription_internal> m_socialRelationshipChangeSubscription;
    peoplehub_service m_peoplehubService;
    xbox_live_callback<void> m_graphDestructionCompleteCallback;
    std::function<void(_In_ xbox::services::real_time_activity::real_time_activity_connection_state state)> m_stateRTAFunction;
    xsapi_internal_unordered_map<uint64_t, xbox_social_user_subscriptions> m_socialUserSubscriptions;
    std::recursive_mutex m_socialGraphMutex;
    std::recursive_mutex m_socialGraphPriorityMutex;
    std::recursive_mutex m_socialGraphStateMutex;
    xbox::services::perf_tester m_perfTester;
    event_queue m_socialEventQueue;
    unprocessed_event_queue m_unprocessedEventQueue;
    user_buffers_holder m_userBuffer;
    async_queue_handle_t m_backgroundAsyncQueue;
};

class xbox_social_user_group_internal
{
public:
    _XSAPIIMP const xsapi_internal_vector<xbox_social_user*>& users();

    _XSAPIIMP xsapi_internal_vector<xbox_social_user> get_copy_of_users();

    _XSAPIIMP social_user_group_type social_user_group_type();

    _XSAPIIMP const xsapi_internal_vector<xbox_user_id_container>& users_tracked_by_social_user_group();

    _XSAPIIMP const xbox_live_user_t& local_user();

    _XSAPIIMP presence_filter presence_filter_of_group() { return m_presenceFilter; }

    _XSAPIIMP relationship_filter relationship_filter_of_group() { return m_relationshipFilter; }

    _XSAPIIMP xsapi_internal_vector<xbox_social_user*> get_users_from_xbox_user_ids(
        _In_ const xsapi_internal_vector<xbox_user_id_container>& xboxUserIds
        );

    xbox_social_user_group_internal(
        _In_ xsapi_internal_string viewHash,
        _In_ presence_filter presenceFilter,
        _In_ relationship_filter relationshipFilter,
        _In_ uint32_t titleId,
        _In_ xbox_live_user_t xboxLiveUser
        );

    xbox_social_user_group_internal(
        _In_ xsapi_internal_string viewHash,
        _In_ xsapi_internal_vector<xsapi_internal_string> userList,
        _In_ xbox_live_user_t xboxLiveUser
        );

#if defined(XSAPI_CPPWINRT)
#if TV_API
    _XSAPIIMP const winrt::Windows::Xbox::System::User& local_user_cppwinrt()
    {
        return convert_user_to_cppwinrt(local_user());
    }
#endif
#endif
private:
    void destroy();

    const xsapi_internal_string& hash() const;

    const xsapi_internal_vector<uint64_t>& tracking_users();

    void update_view(
        _In_ const xsapi_internal_unordered_map<uint64_t, xbox_social_user_context>& snapshotList,
        _In_ const xsapi_internal_vector<std::shared_ptr<social_event_internal>>& socialEvents
        );

    void initialize_filter_list(
        _In_ const xsapi_internal_unordered_map<uint64_t, xbox_social_user_context>& users
        );

    void filter_list(
        _In_ const xsapi_internal_unordered_map<uint64_t, xbox_social_user_context>& snapshotList,
        _In_ const xsapi_internal_vector<std::shared_ptr<social_event_internal>>& socialEvents
        );

    bool get_presence_filter_result(
        _In_ const xbox_social_user* user,
        _In_ presence_filter presenceFilter
        ) const;

    bool needs_update();

    void remove_users(_In_ const xsapi_internal_vector<xbox_removal_struct>& usersToRemove);

    user_group_status_change update_users_in_group(_In_ const xsapi_internal_vector<xsapi_internal_string>& userList);

    bool m_needsUpdate;
    xbox::services::social::manager::social_user_group_type m_userGroupType;
    social_manager_extra_detail_level m_detailLevel;
    presence_filter m_presenceFilter;
    relationship_filter m_relationshipFilter;
    uint32_t m_titleId;
    xbox_live_user_t m_xboxLiveUser;
    xsapi_internal_vector<xbox_user_id_container> m_userUpdateListString;
    xsapi_internal_vector<xbox_social_user*> m_userGroupVector;
    xsapi_internal_vector<uint64_t> m_userUpdateListInt;
    xsapi_internal_string m_viewHash;
    std::mutex m_groupMutex;

    friend class social_manager_internal;
};

class social_manager_internal: public std::enable_shared_from_this<social_manager_internal>
{
public:
    _XSAPIIMP static std::shared_ptr<social_manager_internal> get_singleton_instance();

    _XSAPIIMP virtual xbox_live_result<void> add_local_user(
        _In_ xbox_live_user_t user,
        _In_ social_manager_extra_detail_level extraDetailLevel
        );

    _XSAPIIMP virtual xbox_live_result<void> remove_local_user(
        _In_ xbox_live_user_t user
        );

    _XSAPIIMP xsapi_internal_vector<std::shared_ptr<social_event_internal>> do_work();

    _XSAPIIMP xbox_live_result<std::shared_ptr<xbox_social_user_group_internal>> create_social_user_group_from_filters(
        _In_ xbox_live_user_t user,
        _In_ presence_filter presenceDetailLevel,
        _In_ relationship_filter filter
        );

    _XSAPIIMP xbox_live_result<std::shared_ptr<xbox_social_user_group_internal>> create_social_user_group_from_list(
        _In_ xbox_live_user_t user,
        _In_ xsapi_internal_vector<xsapi_internal_string> xboxUserIdList
        );

    _XSAPIIMP xbox_live_result<void> destroy_social_user_group(
        _In_ std::shared_ptr<xbox_social_user_group_internal> socialUserGroup
        );

    _XSAPIIMP const xsapi_internal_vector<xbox_live_user_t>& local_users() const;

    _XSAPIIMP xbox_live_result<void> update_social_user_group(
        _In_ const std::shared_ptr<xbox_social_user_group_internal>& group,
        _In_ const xsapi_internal_vector<xsapi_internal_string>& users
        );

    _XSAPIIMP xbox_live_result<void> set_rich_presence_polling_status(
        _In_ xbox_live_user_t user,
        _In_ bool shouldEnablePolling
        );

    _XSAPIIMP void set_social_graph_background_async_queue(
        async_queue_handle_t queue
        );

    _XSAPIIMP xbox_services_diagnostics_trace_level diagnostics_trace_level() const;

    _XSAPIIMP void set_diagnostics_trace_level(
        _In_ xbox_services_diagnostics_trace_level traceLevel
    );

    void log_state();

#if defined(XSAPI_CPPWINRT)
#if TV_API
    _XSAPIIMP virtual xbox_live_result<void> add_local_user(
        _In_ const winrt::Windows::Xbox::System::User& user,
        _In_ social_manager_extra_detail_level extraDetailLevel
        )
    {
        return add_local_user(convert_user_to_cppcx(user), extraDetailLevel);
    }

    _XSAPIIMP virtual xbox_live_result<void> remove_local_user(
        _In_ const winrt::Windows::Xbox::System::User& user
        )
    {
        return remove_local_user(convert_user_to_cppcx(user));
    }

    _XSAPIIMP xbox_live_result<std::shared_ptr<xbox_social_user_group_internal>> create_social_user_group_from_filters(
        _In_ const winrt::Windows::Xbox::System::User& user,
        _In_ presence_filter presenceDetailLevel,
        _In_ relationship_filter filter
        )
    {
        return create_social_user_group_from_filters(convert_user_to_cppcx(user), presenceDetailLevel, filter);
    }

    _XSAPIIMP xbox_live_result<std::shared_ptr<xbox_social_user_group_internal>> create_social_user_group_from_list(
        _In_ const winrt::Windows::Xbox::System::User& user,
        _In_ xsapi_internal_vector<xsapi_internal_string> xboxUserIdList
        )
    {
        return create_social_user_group_from_list(convert_user_to_cppcx(user), xboxUserIdList);
    }

    _XSAPIIMP const std::vector<winrt::Windows::Xbox::System::User>& local_users_cppwinrt() const
    {
        return convert_user_vector_to_cppwinrt(local_users());
    }
#endif
#endif

protected:
    social_manager_internal();

    xbox_services_diagnostics_trace_level m_traceLevel;

    std::recursive_mutex m_socialMangerLock;
    std::recursive_mutex m_socialManagerEventLock;

    xsapi_internal_vector<std::shared_ptr<social_event_internal>> m_eventQueue;
    xsapi_internal_vector<xbox_live_user_t> m_localUserList;
    xsapi_internal_unordered_map<xsapi_internal_string, std::shared_ptr<xbox_social_user_group_internal>> m_xboxSocialUserGroups;
    xsapi_internal_unordered_map<xsapi_internal_string, xsapi_internal_vector<xsapi_internal_string>> m_userToViewMap;
    xsapi_internal_unordered_map<xsapi_internal_string, std::shared_ptr<social_graph>> m_localGraphs;

    async_queue_handle_t m_backgroundAsyncQueue;

    friend class xbox_social_user_group_internal;
    friend class MockSocialManager;
};

}}}}

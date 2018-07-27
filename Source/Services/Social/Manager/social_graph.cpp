// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_manager_internal.h"
#include "xsapi/services.h"
#include "xsapi/system.h"
#include "xsapi/presence.h"
#include "xbox_live_context_impl.h"
#include "system_internal.h"
#include "xbox_system_factory.h"
#include "social_internal.h"
#include "xbox_live_app_config_internal.h"
#include "presence_internal.h"

using namespace xbox::services;
using namespace xbox::services::system;
using namespace xbox::services::presence;
using namespace xbox::services::social;
using namespace xbox::services::real_time_activity;
using namespace Concurrency::extras;

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

const std::chrono::seconds social_graph::TIME_PER_CALL_SEC =
#if UNIT_TEST_SERVICES
std::chrono::seconds::zero();
#else
std::chrono::seconds(30);
#endif

const std::chrono::minutes social_graph::REFRESH_TIME_MIN = std::chrono::minutes(20);
const uint32_t social_graph::NUM_EVENTS_PER_FRAME = 5;

social_graph::social_graph(
    _In_ xbox_live_user_t user,
    _In_ social_manager_extra_detail_level socialManagerExtraDetailLevel,
    _In_ xbox_live_callback<void> graphDestructionCompleteCallback,
    _In_ async_queue_handle_t backgroundAsyncQueue
    ) :
    m_detailLevel(socialManagerExtraDetailLevel),
    m_xboxLiveContextImpl(new xbox_live_context_impl(m_user)),
    m_user(std::move(user)),
    m_graphDestructionCompleteCallback(std::move(graphDestructionCompleteCallback)),
    m_isInitialized(false),
    m_socialGraphState(social_graph_state::normal),
    m_stateRTAFunction(nullptr),
    m_perfTester("social_graph"),
    m_wasDisconnected(false),
    m_numEventsThisFrame(0),
    m_userAddedContext(0),
    m_shouldCancel(false),
    m_isPollingRichPresence(false),
    m_backgroundAsyncQueue(backgroundAsyncQueue)
{
    m_xboxLiveContextImpl->user_context()->set_caller_context_type(caller_context_type::social_manager);
    m_xboxLiveContextImpl->init();
    m_peoplehubService = peoplehub_service(
        m_xboxLiveContextImpl->user_context(),
        m_xboxLiveContextImpl->settings(),
        xbox_live_app_config_internal::get_app_config_singleton()
        );

    LOG_DEBUG_IF(
        social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::verbose,
        "social_graph created"
    );
}

social_graph::~social_graph()
{
    std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
    std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);
    m_xboxLiveContextImpl->real_time_activity_service()->deactivate();

    m_perfTester.start_timer("~social_graph");
    try
    {
        if (m_graphDestructionCompleteCallback != nullptr)
        {
            m_graphDestructionCompleteCallback();
        }
    }
    catch (...)
    {
        LOG_ERROR_IF(get_xsapi_singleton(false) && 
            social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
            "Exception happened during graph destruction complete callback"
            );
    }

    LOG_DEBUG_IF(get_xsapi_singleton(false) &&
        social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::verbose,
        "social_graph destroyed"
        );

    m_perfTester.stop_timer("~social_graph");
}

void social_graph::initialize(xbox_live_callback<xbox_live_result<void>> callback) 
{
    std::weak_ptr<social_graph> thisWeakPtr = shared_from_this();
    setup_rta();

    m_presenceRefreshTimer = xsapi_allocate_shared<call_buffer_timer>(
    [thisWeakPtr](const xsapi_internal_vector<xsapi_internal_string>& eventArgs, std::shared_ptr<call_buffer_timer_completion_context>)
    {
        std::shared_ptr<social_graph> pThis(thisWeakPtr.lock());
        if (pThis)
        {
            pThis->presence_timer_callback(
                eventArgs
            );
        }
    },
        TIME_PER_CALL_SEC,
        m_backgroundAsyncQueue
        );

    m_presencePollingTimer = xsapi_allocate_shared<call_buffer_timer>(
    [thisWeakPtr](const xsapi_internal_vector<xsapi_internal_string> eventArgs, std::shared_ptr<call_buffer_timer_completion_context>)
    {
        std::shared_ptr<social_graph> pThis(thisWeakPtr.lock());
        if (pThis)
        {
            pThis->presence_timer_callback(
                eventArgs
            );
        }
    },
        TIME_PER_CALL_SEC,
        m_backgroundAsyncQueue
        );

    m_socialGraphRefreshTimer = xsapi_allocate_shared<call_buffer_timer>(
    [thisWeakPtr](const xsapi_internal_vector<xsapi_internal_string> eventArgs, std::shared_ptr<call_buffer_timer_completion_context> completionContext)
    {
        std::shared_ptr<social_graph> pThis(thisWeakPtr.lock());
        if (pThis)
        {
            pThis->social_graph_timer_callback(
                eventArgs,
                completionContext
                );
        }
    },
        TIME_PER_CALL_SEC,
        m_backgroundAsyncQueue
        );

    m_resyncRefreshTimer = xsapi_allocate_shared<call_buffer_timer>(
    [thisWeakPtr](xsapi_internal_vector<xsapi_internal_string>, std::shared_ptr<call_buffer_timer_completion_context>)
    {
        std::shared_ptr<social_graph> pThis(thisWeakPtr.lock());
        if (pThis)
        {
            pThis->refresh_graph();
        }
    },
        TIME_PER_CALL_SEC,
        m_backgroundAsyncQueue
        );

#if UWP_API || TV_API || UNIT_TEST_SERVICES
    schedule_social_graph_refresh();
#endif
    schedule_event_work();

    m_peoplehubService.get_social_graph(
#if TV_API || UNIT_TEST_SERVICES || !XSAPI_CPP
        utils::internal_string_from_string_t(m_xboxLiveContextImpl->user()->XboxUserId->Data()),
#else
        utils::internal_string_from_string_t(m_xboxLiveContextImpl->user()->xbox_user_id()),
#endif
        m_detailLevel,
        m_backgroundAsyncQueue,
        [thisWeakPtr, callback](xbox_live_result<xsapi_internal_vector<xbox_social_user>> socialUsersResult)
    {
        try
        {
            std::shared_ptr<social_graph> pThis(thisWeakPtr.lock());
            if (pThis)
            {
                // Since this is initializing the social graph a 424 is allowed
                if (socialUsersResult.err() != xbox_live_error_code::no_error && socialUsersResult.err() != xbox_live_error_code::http_status_424_failed_dependency)
                {
                    callback(xbox_live_result<void>(socialUsersResult.err(), socialUsersResult.err_message()));
                    return;
                }

                pThis->initialize_social_buffers(socialUsersResult.payload());
                auto& inactiveBufferSocialGraph = pThis->m_userBuffer.inactive_buffer()->socialUserGraph;
                for (auto& user : inactiveBufferSocialGraph)
                {
                    if (user.second.socialUser == nullptr)
                        continue;

                    auto devicePresenceSubResult = pThis->m_xboxLiveContextImpl->presence_service()->subscribe_to_device_presence_change(utils::internal_string_from_string_t(user.second.socialUser->xbox_user_id()));
                    auto titlePresenceSubResult = pThis->m_xboxLiveContextImpl->presence_service()->subscribe_to_title_presence_change(
                        utils::internal_string_from_char_t(user.second.socialUser->xbox_user_id()),
                        pThis->m_xboxLiveContextImpl->application_config()->title_id()
                        );

                    if (devicePresenceSubResult.err() || titlePresenceSubResult.err())
                    {
                        callback(xbox_live_result<void>(xbox_live_error_code::runtime_error, "subscription initialization failed"));
                        return;
                    }

                    std::lock_guard<std::recursive_mutex> lock(pThis->m_socialGraphMutex);
                    std::lock_guard<std::recursive_mutex> priorityLock(pThis->m_socialGraphPriorityMutex);
                    pThis->m_perfTester.start_timer("sub");
                    pThis->m_socialUserSubscriptions[user.first].devicePresenceChangeSubscription = devicePresenceSubResult.payload();
                    pThis->m_socialUserSubscriptions[user.first].titlePresenceChangeSubscription = titlePresenceSubResult.payload();
                    pThis->m_perfTester.stop_timer("sub");
                }


                std::lock_guard<std::recursive_mutex> lock(pThis->m_socialGraphMutex);
                std::lock_guard<std::recursive_mutex> priorityLock(pThis->m_socialGraphPriorityMutex);
                pThis->m_perfTester.start_timer("m_isInitialized");
                pThis->m_isInitialized = true;
                pThis->m_perfTester.stop_timer("m_isInitialized");
            }
            else
            {
                callback(xbox_live_result<void>(xbox_live_error_code::runtime_error));
                return;
            }
        }
        catch (const std::exception& e)
        {
            LOGS_DEBUG_IF(social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::verbose) 
                << "Exception in get_social_graph " << e.what();
        }
        catch (...)
        {
            LOG_DEBUG_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::verbose,
                "Unknown std::exception in initialization"
            );
        }
        callback(xbox_live_result<void>());
    });
}

const xsapi_internal_unordered_map<uint64_t, xbox_social_user_context>*
social_graph::active_buffer_social_graph()
{
    std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
    std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);
    return &m_userBuffer.active_buffer()->socialUserGraph;
}

void
social_graph::set_background_async_queue(async_queue_handle_t queue)
{
    m_backgroundAsyncQueue = queue;
}

void 
social_graph::schedule_event_work()
{
    std::weak_ptr<social_graph> thisWeak = shared_from_this();

    AsyncBlock* async = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
    async->context = utils::store_weak_ptr(thisWeak);
    async->queue = m_backgroundAsyncQueue;
    async->callback = [](AsyncBlock* async)
    {
        auto pThis = utils::get_shared_ptr<social_graph>(async->context);
        if (pThis)
        {
            pThis->schedule_event_work();
        }
        xsapi_memory::mem_free(async);
    };

    BeginAsync(async, utils::store_weak_ptr(thisWeak), nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        if (op == AsyncOp_DoWork)
        {
            auto pThis = utils::get_shared_ptr<social_graph>(data->context);
            if (pThis)
            {
                bool hasRemainingEvent = false;
                do
                {
                    hasRemainingEvent = pThis->do_event_work();
                } while (hasRemainingEvent);
            }
            CompleteAsync(data->async, S_OK, 0);
        }
        return S_OK;
    });
    ScheduleAsync(async, 30);
}

bool
social_graph::do_event_work()
{
    bool hasRemainingEvent = false;
    bool hasCachedEvents = false;
    {
        std::lock_guard<std::recursive_mutex> socialGraphStateLock(m_socialGraphStateMutex);
        {
            std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
            std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);
            set_state(social_graph_state::event_processing);

            m_perfTester.start_timer("do_event_work: event_processing");
            m_perfTester.start_timer("do_event_work: has_cached_events");
            hasCachedEvents = m_isInitialized && m_userBuffer.inactive_buffer() && !m_userBuffer.inactive_buffer()->socialUserEventQueue.empty(true);
            m_perfTester.stop_timer("do_event_work: has_cached_events");
            if (hasCachedEvents)
            {
                m_perfTester.start_timer("do_event_work: set_state");
                LOG_INFO_IF(
                    social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::info,
                    "set state: event_processing"
                );
                m_perfTester.stop_timer("do_event_work: set_state");
            }
            m_perfTester.stop_timer("do_event_work: event_processing");
        }
        if (hasCachedEvents)
        {
            process_cached_events();
            hasRemainingEvent = true;
        }
        else if (m_isInitialized)
        {
            std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
            std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);
            m_perfTester.start_timer("do_event_work: process_events");
            set_state(social_graph_state::normal);
            hasRemainingEvent = process_events(); //effectively a coroutine here so that each event yields when it is done processing
            m_perfTester.stop_timer("do_event_work: process_events");
        }
        else
        {
            std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
            std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);

            m_perfTester.start_timer("set_state: normal");
            set_state(social_graph_state::normal);
            m_perfTester.stop_timer("set_state: normal");
        }
    }

    return hasRemainingEvent;
}

void social_graph::initialize_social_buffers(
    _In_ const xsapi_internal_vector<xbox_social_user>& socialUsers
    )
{
    m_userBuffer.initialize(socialUsers);
}

bool
social_graph::is_initialized()
{
    std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
    std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);
    return m_isInitialized;
}

void
social_graph::process_cached_events()
{
    if (m_userBuffer.inactive_buffer() != nullptr)
    {
        auto inactiveBuffer = m_userBuffer.inactive_buffer();
        auto& eventQueue = inactiveBuffer->socialUserEventQueue;
        while (!eventQueue.empty())
        {
            auto evt = eventQueue.pop();
            apply_event(evt, false);
        }

        std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
        std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);
        set_state(social_graph_state::normal);
    }
}

bool
social_graph::process_events()
{
    bool shouldApplyEvent = !m_unprocessedEventQueue.empty() && m_numEventsThisFrame < NUM_EVENTS_PER_FRAME;
    if(shouldApplyEvent)
    {
        ++m_numEventsThisFrame;
        auto evt = m_unprocessedEventQueue.pop();
        apply_event(evt, true);
        m_userBuffer.add_event(evt);
    }

    return shouldApplyEvent;
}

void
social_graph::apply_event(
    _In_ const unprocessed_social_event& evt,
    _In_ bool isFreshEvent
    )
{
    const auto& inactiveBuffer = m_userBuffer.inactive_buffer();
    if (inactiveBuffer == nullptr)
    {
        LOG_ERROR_IF(
            social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
            "In active buffer null in event processing"
        );
        return;
    }

    social_event_type eventType = social_event_type::unknown;
    switch (evt.event_type())
    {
        case unprocessed_social_event_type::users_added:
        {
            LOG_INFO_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::info, 
                "Applying internal events: users_added"
            );
            apply_users_added_event(evt, inactiveBuffer, isFreshEvent);
            break;
        }
        case unprocessed_social_event_type::users_changed:
        {
            LOG_INFO_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::info,
                "Applying internal events: users_changed"
            );
            apply_users_change_event(evt, inactiveBuffer, isFreshEvent);
            break;
        }
        case unprocessed_social_event_type::users_removed:
        {
            LOG_INFO_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::info,
                "Applying internal events: users_removed"
            );
            apply_users_removed_event(evt, inactiveBuffer, eventType, isFreshEvent);
            break;
        }
        case unprocessed_social_event_type::device_presence_changed:
        {
            LOG_INFO_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::info,
                "Applying internal events: device_presence_changed"
            );

            apply_device_presence_changed_event(evt, inactiveBuffer, isFreshEvent, eventType);
            break;
        }
        case unprocessed_social_event_type::title_presence_changed:
        {
            LOG_INFO_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::info, 
                "Applying internal events: title_presence_changed"
            );
            auto titlePresenceChanged = evt.title_presence_args();
            auto xuid = utils::internal_string_to_uint64(titlePresenceChanged->xbox_user_id());
            auto xuidIter = inactiveBuffer->socialUserGraph.find(xuid);
            if (xuidIter == inactiveBuffer->socialUserGraph.end() || (xuidIter != inactiveBuffer->socialUserGraph.end() && xuidIter->second.socialUser == nullptr))
            {
                LOG_ERROR_IF(
                    social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                    "social graph: social user not found in title presence change"
                );
                break;
            }
            auto& userPresenceRecord = inactiveBuffer->socialUserGraph.at(xuid).socialUser->m_presenceRecord;
            if (titlePresenceChanged->title_state() == xbox::services::presence::title_presence_state::ended)
            {
                userPresenceRecord._Remove_title(
                    titlePresenceChanged->title_id()
                );
            }

            eventType = social_event_type::presence_changed;
            break;
        }
        case unprocessed_social_event_type::presence_changed:
        {
            LOG_INFO_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::info,
                "Applying internal events: presence_changed"
            );
            apply_presence_changed_event(evt, inactiveBuffer, isFreshEvent);
            break;
        }
        case unprocessed_social_event_type::social_relationships_changed:
        case unprocessed_social_event_type::profiles_changed:
        {
            LOG_INFO_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::info,
                "Applying internal events: social_relationships_changed or profiles_changed"
            );
            m_perfTester.start_timer("profiles_changed");
            for (auto& user : evt.users_affected())
            {
                auto userIterator = inactiveBuffer->socialUserGraph.find(user._Xbox_user_id_as_integer());
                if (userIterator != inactiveBuffer->socialUserGraph.end() && userIterator->second.socialUser != nullptr)
                {
                    *(userIterator->second.socialUser) = user;
                }
            }

            eventType = social_event_type::profiles_changed;
            m_perfTester.stop_timer("profiles_changed");
            break;
        }
        case unprocessed_social_event_type::unknown:
        default:
        {
            LOG_ERROR_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                "unknown event in process_events"
            );
        }
    }

    if (isFreshEvent)
    {
        m_socialEventQueue.push(evt, m_user, eventType);
    }
}

void social_graph::apply_users_added_event(
    _In_ const unprocessed_social_event& evt,
    _In_ user_buffer* inactiveBuffer,
    _In_ bool isFreshEvent
    )
{
    m_perfTester.start_timer("apply_users_added_event");
    xsapi_internal_vector<xsapi_internal_string> usersToAdd;
    for (auto& user : evt.users_affected_as_string_vec())
    {
        auto userIter = inactiveBuffer->socialUserGraph.find(utils::internal_string_to_uint64(user));
        if (userIter != inactiveBuffer->socialUserGraph.end())
        {
            ++userIter->second.refCount;
        }
        else
        {
            usersToAdd.push_back(user);
        }
    }

    if (usersToAdd.empty())
    {
        invoke_callback(evt.callback, xbox_live_result<void>());
    }
    else
    {
        auto usersAddedStruct = xsapi_allocate_shared<call_buffer_timer_completion_context>(
            ++m_userAddedContext,
            usersToAdd.size(),
            evt.callback
            );

        if (isFreshEvent)
        {
            m_socialGraphRefreshTimer->fire(usersToAdd, usersAddedStruct);
        }

        for (auto& user : usersToAdd)
        {
            auto userAsInt = utils::internal_string_to_uint64(user);
            inactiveBuffer->socialUserGraph[userAsInt].socialUser = nullptr;
            inactiveBuffer->socialUserGraph[userAsInt].refCount = 1;
        }
    }
    m_perfTester.stop_timer("apply_users_added_event");
}

void
social_graph::apply_users_removed_event(
    _In_ const unprocessed_social_event& evt,
    _In_ user_buffer* inactiveBuffer,
    _Inout_ social_event_type& eventType,
    _In_ bool isFreshEvent
    )
{
    m_perfTester.start_timer("removing_users");
    auto usersAffected = evt.users_to_remove();
    xsapi_internal_vector<uint64_t> removeUsers;
    for (auto& user : usersAffected)
    {
        --inactiveBuffer->socialUserGraph[user].refCount;
        if (inactiveBuffer->socialUserGraph[user].refCount == 0)
        {
            if (inactiveBuffer->socialUserGraph[user].socialUser != nullptr)
            {
                removeUsers.push_back(user);
            }
            else
            {
                inactiveBuffer->socialUserGraph.erase(user);
            }

            eventType = social_event_type::users_removed_from_social_graph;
        }
    }

    m_userBuffer.remove_users_from_buffer(removeUsers, *inactiveBuffer);
    if (isFreshEvent)
    {
        unsubscribe_users(removeUsers);
    }
    m_perfTester.stop_timer("removing_users");
}

void
social_graph::apply_users_change_event(
    _In_ const unprocessed_social_event& evt,
    _In_ user_buffer* inactiveBuffer,
    _In_ bool isFreshEvent
    )
{
    m_perfTester.start_timer("apply_users_change_event");
    xsapi_internal_vector<xbox_social_user> usersToAdd;
    xsapi_internal_vector<xbox_social_user> usersChanged;
    if (evt.completion_context() != nullptr)
    {
        // delay callback invocation to avoid deadlock
        invoke_callback(evt.completion_context()->callback, evt.error());
    }

    auto result = evt.error();
    if (result.err() != xbox_live_error_code::no_error)
    {
        m_socialEventQueue.push(evt, m_user, social_event_type::users_added_to_social_graph, evt.error());
        return;
    }

    for (auto user : evt.users_affected())
    {
        auto userIter = inactiveBuffer->socialUserGraph.find(user._Xbox_user_id_as_integer());
        if (userIter != inactiveBuffer->socialUserGraph.end())  // if not found then it was deleted while the lookup was happening
        {
            if (userIter->second.socialUser == nullptr)
            {
                usersToAdd.push_back(user);
            }
            else
            {
                *userIter->second.socialUser = user;
                usersChanged.push_back(user);
            }
        }
    }

    if (!usersToAdd.empty())
    {
        auto numObjects = evt.completion_context() != nullptr ? evt.completion_context()->numObjects : 0;
        m_userBuffer.add_users_to_buffer(usersToAdd, *inactiveBuffer, numObjects);

        xsapi_internal_vector<uint64_t> usersList;
        for (auto& user : usersToAdd)
        {
            usersList.push_back(user._Xbox_user_id_as_integer());
        }
        if (isFreshEvent)
        {
            setup_device_and_presence_subscriptions(usersList);
            unprocessed_social_event internalSocialUsersAddedEvent(unprocessed_social_event_type::users_added, usersToAdd);
            m_socialEventQueue.push(internalSocialUsersAddedEvent, m_user, social_event_type::users_added_to_social_graph);
        }
    }

    if (!usersChanged.empty() && isFreshEvent)
    {
        unprocessed_social_event internalSocialProfileChangedEvent(unprocessed_social_event_type::profiles_changed, usersChanged);
        m_socialEventQueue.push(internalSocialProfileChangedEvent, m_user, social_event_type::profiles_changed);
    }

    m_perfTester.stop_timer("apply_users_change_event");
}

void social_graph::apply_device_presence_changed_event(
    _In_ const unprocessed_social_event& evt,
    _In_ user_buffer* inactiveBuffer,
    _In_ bool isFreshEvent,
    _Inout_ social_event_type& eventType
    )
{
    m_perfTester.start_timer("apply_device_presence_changed_event");
    auto devicePresenceChangedArgs = evt.device_presence_args();
    auto xuid = utils::internal_string_to_uint64(devicePresenceChangedArgs->xbox_user_id());

    bool fireCallbackTimer = false;
    auto mapIter = m_userBuffer.inactive_buffer()->socialUserGraph.find(xuid);
    if (mapIter != m_userBuffer.inactive_buffer()->socialUserGraph.end())
    {
        if (mapIter->second.socialUser == nullptr)
        {
            LOG_ERROR_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                "social graph: social user null in apply_device_presence_changed_event"
            );
            return;
        }
        auto& userPresenceRecord = mapIter->second.socialUser->presence_record();
        auto deviceRecordSize = userPresenceRecord.presence_title_records().size();
        fireCallbackTimer = deviceRecordSize > 1 || devicePresenceChangedArgs->is_user_logged_on_device();
    }
    else
    {
        LOG_ERROR_IF(
            social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
            "device presence record received for user not in graph"
        );
        return;
    }

    if (fireCallbackTimer && isFreshEvent)
    {
        xsapi_internal_vector<xsapi_internal_string> entryVec(1, devicePresenceChangedArgs->xbox_user_id());
        m_presenceRefreshTimer->fire(entryVec);
    }
    else if (!fireCallbackTimer)
    {
        auto xuidIter = inactiveBuffer->socialUserGraph.find(xuid);
        if (xuidIter == inactiveBuffer->socialUserGraph.end() || (xuidIter != inactiveBuffer->socialUserGraph.end() && xuidIter->second.socialUser == nullptr))
        {
            LOG_ERROR_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                "social graph: social user null in inactiveBuffer"
            );
            return;
        }
        auto& userPresenceRecord = xuidIter->second.socialUser->m_presenceRecord;
        userPresenceRecord._Update_device(
            devicePresenceChangedArgs->device_type(),
            devicePresenceChangedArgs->is_user_logged_on_device()
        );

        eventType = social_event_type::presence_changed;
    }
    m_perfTester.stop_timer("apply_device_presence_changed_event");
}

void social_graph::apply_presence_changed_event(
    _In_ const unprocessed_social_event& evt,
    _In_ user_buffer* inactiveBuffer,
    _In_ bool isFreshEvent
    )
{
    m_perfTester.start_timer("apply_presence_changed_event");
    xsapi_internal_vector<uint64_t> userAddedVec;
    auto& presenceRecords = evt.presence_records();
    for (auto& presenceRecord : presenceRecords)
    {
        uint64_t index = presenceRecord._Xbox_user_id();
        if (index == 0)
        {
            LOG_ERROR_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                "social_graph: Invalid user in apply_presence_changed_event"
            );
            continue;
        }

        auto userPresenceRecordIter = inactiveBuffer->socialUserGraph.find(index);
        if (userPresenceRecordIter != inactiveBuffer->socialUserGraph.end())
        {
            auto socialUser = userPresenceRecordIter->second.socialUser;
            if (socialUser == nullptr)
            {
                LOG_ERROR_IF(
                    social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                    "social_graph: User not found in updating presence"
                );
                continue;
            }
            auto userPresenceRecord = socialUser->presence_record();
            if (userPresenceRecord._Compare(presenceRecord))    // TODO: potential optimization, limits the number of compares that can happen in a single event (i.e. if presence result has 100 record split it up into 10 events)
            {
                auto socialUserGraphUser = inactiveBuffer->socialUserGraph.at(presenceRecord._Xbox_user_id()).socialUser;
                if (socialUserGraphUser == nullptr)
                {
                    LOG_ERROR_IF(
                        social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                        "social_graph: User not found in social user graph"
                    );
                    continue;
                }
                socialUserGraphUser->_Set_presence_record(presenceRecord);
                userAddedVec.push_back(presenceRecord._Xbox_user_id());
            }
        }
    }

    if (isFreshEvent && !userAddedVec.empty())
    {
        unprocessed_social_event internalPresenceChangedEvent(unprocessed_social_event_type::presence_changed, userAddedVec);
        m_socialEventQueue.push(internalPresenceChangedEvent, m_user, social_event_type::presence_changed);
    }

    m_perfTester.stop_timer("apply_presence_changed_event");
}

void
social_graph::set_state(
    _In_ social_graph_state socialGraphState
    )
{
    m_socialGraphState = socialGraphState;
}

void
social_graph::setup_rta()
{
    std::weak_ptr<social_graph> thisWeakPtr = shared_from_this();

    setup_rta_subscriptions();

    m_devicePresenceContext = m_xboxLiveContextImpl->presence_service()->add_device_presence_changed_handler(
        [thisWeakPtr](std::shared_ptr<device_presence_change_event_args_internal> eventArgs)
    {
        std::shared_ptr<social_graph> pThis(thisWeakPtr.lock());
        if (pThis)
        {
            pThis->handle_device_presence_change(
                eventArgs
                );
        }
    });

    m_titlePresenceContext = m_xboxLiveContextImpl->presence_service()->add_title_presence_changed_handler(
        [thisWeakPtr](std::shared_ptr<title_presence_change_event_args_internal> eventArgs)
    {
        std::shared_ptr<social_graph> pThis(thisWeakPtr.lock());
        if (pThis)
        {
            pThis->handle_title_presence_change(
                eventArgs
                );
        }
    });

    m_socialRelationshipContext = m_xboxLiveContextImpl->social_service_impl()->add_social_relationship_changed_handler(
        [thisWeakPtr](std::shared_ptr<social_relationship_change_event_args_internal> eventArgs)
    {
        std::shared_ptr<social_graph> pThis(thisWeakPtr.lock());
        if (pThis)
        {
            pThis->handle_social_relationship_change(
                eventArgs
                );
        }
    });
}

void
social_graph::setup_rta_subscriptions(
    _In_ bool shouldReinitialize
    )
{
    std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
    std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);
    m_perfTester.start_timer("setup_rta_subscriptions");
    m_xboxLiveContextImpl->real_time_activity_service()->activate();
    auto socialRelationshipChangeResult = m_xboxLiveContextImpl->social_service_impl()->subscribe_to_social_relationship_change(
        m_xboxLiveContextImpl->xbox_live_user_id()
        );

    if (socialRelationshipChangeResult.err())
    {
        LOGS_ERROR_IF(social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error)
            << "Social relationship change error " << socialRelationshipChangeResult.err().message() << " message: " << socialRelationshipChangeResult.err_message();
    }
    else
    {
        m_socialRelationshipChangeSubscription = socialRelationshipChangeResult.payload();
    }

    if (shouldReinitialize)
    {
        if (m_userBuffer.inactive_buffer() == nullptr)
        {
            LOG_ERROR_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                "Failed to reinitialize rta subs"
            );
            return;
        }
        xsapi_internal_vector<uint64_t> users;
        for (auto& userPair : m_userBuffer.inactive_buffer()->socialUserGraph)
        {
            auto user = userPair.second.socialUser;
            if (user == nullptr)
            {
                LOG_ERROR_IF(
                    social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                    "social_graph: setup_rta_subscriptions get users"
                );
                continue;
            }

            users.push_back(user->_Xbox_user_id_as_integer());
        }

        setup_device_and_presence_subscriptions(users);
    }

    std::weak_ptr<social_graph> thisWeakPtr = shared_from_this();
    m_resyncContext = m_xboxLiveContextImpl->real_time_activity_service()->add_resync_handler(
        [thisWeakPtr]()
    {
        std::shared_ptr<social_graph> pThis(thisWeakPtr.lock());
        if (pThis)
        {
            pThis->m_resyncRefreshTimer->fire();
        }
    });

    m_subscriptionErrorContext = m_xboxLiveContextImpl->real_time_activity_service()->add_subscription_error_handler(
        [thisWeakPtr](real_time_activity_subscription_error_event_args args)
    {
        std::shared_ptr<social_graph> pThis(thisWeakPtr.lock());
        if (pThis)
        {
            pThis->handle_rta_subscription_error(args);
        }
    });

    m_rtaStateChangeContext = m_xboxLiveContextImpl->real_time_activity_service()->add_connection_state_change_handler(
        [thisWeakPtr](real_time_activity_connection_state args)
    {
        std::shared_ptr<social_graph> pThis(thisWeakPtr.lock());
        if (pThis)
        {
            pThis->handle_rta_connection_state_change(args);
        }
    });

    m_perfTester.stop_timer("setup_rta_subscriptions");
}

void
social_graph::setup_device_and_presence_subscriptions_helper(
    _In_ const xsapi_internal_vector<uint64_t>& users
    )
{
    for (auto xuid : users)
    {
        xsapi_internal_stringstream str;
        str << xuid;
        auto xuidStr = str.str();

        auto devicePresenceSubResult = m_xboxLiveContextImpl->presence_service()->subscribe_to_device_presence_change(xuidStr);
        auto titlePresenceSubResult = m_xboxLiveContextImpl->presence_service()->subscribe_to_title_presence_change(
            xuidStr,
            m_xboxLiveContextImpl->application_config()->title_id()
            );

        if (devicePresenceSubResult.err() || titlePresenceSubResult.err())
        {
            LOG_ERROR_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                "presence subscription failed in social manager"
            );
        }


        std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
        std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);

        m_perfTester.start_timer("setup_device_and_presence_subscriptions");
        m_socialUserSubscriptions[xuid].devicePresenceChangeSubscription = devicePresenceSubResult.payload();
        m_socialUserSubscriptions[xuid].titlePresenceChangeSubscription = titlePresenceSubResult.payload();
        m_perfTester.stop_timer("setup_device_and_presence_subscriptions");
    }
}

struct social_graph_context
{
    social_graph_context(xsapi_internal_vector<uint64_t> _users, std::weak_ptr<social_graph> _pThis)
        : users(std::move(_users)),
        pThis(std::move(_pThis))
    {
    }
    xsapi_internal_vector<uint64_t> users;
    std::weak_ptr<social_graph> pThis;
};

void
social_graph::setup_device_and_presence_subscriptions(
    _In_ const xsapi_internal_vector<uint64_t>& users
)
{
    AsyncBlock* async = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
    async->queue = m_backgroundAsyncQueue;
    async->callback = [](AsyncBlock* asyncBlock)
    {
        xsapi_memory::mem_free(asyncBlock);
    };

    auto context = utils::store_shared_ptr(xsapi_allocate_shared<social_graph_context>(users, shared_from_this()));
    BeginAsync(async, context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {   
        if (op == AsyncOp_DoWork)
        {
            auto context = utils::get_shared_ptr<social_graph_context>(data->context);
            std::shared_ptr<social_graph> pThis(context->pThis.lock());

            if (pThis != nullptr)
            {
                pThis->setup_device_and_presence_subscriptions_helper(context->users);
            }
        }
        return S_OK;
    });
    ScheduleAsync(async, 0);
}

void
social_graph::unsubscribe_users(
    _In_ const xsapi_internal_vector<uint64_t>& users
    )
{
    AsyncBlock* async = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
    async->queue = m_backgroundAsyncQueue;
    async->callback = [](AsyncBlock* asyncBlock)
    {
        xsapi_memory::mem_free(asyncBlock);
    };

    auto context = utils::store_shared_ptr(xsapi_allocate_shared<social_graph_context>(users, shared_from_this()));

    BeginAsync(async, context, nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        if (op == AsyncOp_DoWork)
        {
            auto context = utils::get_shared_ptr<social_graph_context>(data->context);

            std::shared_ptr<social_graph> pThis(context->pThis.lock());
            if (pThis != nullptr)
            {
                for (auto& user : context->users)
                {
                    std::lock_guard<std::recursive_mutex> lock(pThis->m_socialGraphMutex);
                    std::lock_guard<std::recursive_mutex> priorityLock(pThis->m_socialGraphPriorityMutex);
                    pThis->m_perfTester.start_timer("unsubscribe_users");
                    auto subscriptions = pThis->m_socialUserSubscriptions[user];
                    pThis->m_xboxLiveContextImpl->presence_service()->unsubscribe_from_device_presence_change(subscriptions.devicePresenceChangeSubscription);
                    pThis->m_xboxLiveContextImpl->presence_service()->unsubscribe_from_title_presence_change(subscriptions.titlePresenceChangeSubscription);
                    pThis->m_socialUserSubscriptions.erase(user);
                    pThis->m_perfTester.stop_timer("unsubscribe_users");
                }
            }
            CompleteAsync(data->async, S_OK, 0);

            // Have to return E_PENDING from AsyncOp_DoWork
            return E_PENDING;
        }
        return S_OK;
    });
    ScheduleAsync(async, 0);
}

void social_graph::refresh_graph_helper(xsapi_internal_vector<uint64_t>& userRefreshList)
{
    auto inactiveBuffer = m_userBuffer.inactive_buffer();
    if (inactiveBuffer == nullptr)
    {
        return;
    }
    for (auto& user : inactiveBuffer->socialUserGraph)
    {
        auto socialUser = user.second.socialUser;
        if (socialUser == nullptr)
        {
            LOG_ERROR_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                "social graph: no user found in refresh_graph_helper"
            );
            continue;
        }
        if (!socialUser->is_followed_by_caller())
        {
            userRefreshList.push_back(user.first);
        }
    }
}

void
social_graph::refresh_graph()
{
    xsapi_internal_vector<uint64_t> userRefreshList;
    {
        std::lock_guard<std::recursive_mutex> socialGraphStateLock(m_socialGraphStateMutex);
        {
            std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
            std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);

            m_perfTester.start_timer("refresh_graph");
            set_state(social_graph_state::refresh);
            m_perfTester.stop_timer("refresh_graph");
        }
        refresh_graph_helper(userRefreshList);
        {
            std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
            std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);

            m_perfTester.start_timer("refresh_graph stop");
            set_state(social_graph_state::normal);
            m_perfTester.stop_timer("rrefresh_graph stop");
        }
    }

    xsapi_internal_vector<xsapi_internal_string> userRefreshListStr;
    for (auto& user : userRefreshList)
    {
        xsapi_internal_stringstream str;
        str << user;
        userRefreshListStr.push_back(str.str());
    }

    m_socialGraphRefreshTimer->fire(userRefreshListStr);

    std::weak_ptr<social_graph> thisWeakPtr = shared_from_this();
    m_peoplehubService.get_social_graph(
#if TV_API || UNIT_TEST_SERVICES || !XSAPI_CPP
        utils::internal_string_from_string_t(m_xboxLiveContextImpl->user()->XboxUserId->Data()),
#else
        utils::internal_string_from_string_t(m_xboxLiveContextImpl->user()->xbox_user_id()),
#endif
        m_detailLevel,
        m_backgroundAsyncQueue,
        [thisWeakPtr](xbox_live_result<xsapi_internal_vector<xbox_social_user>> socialListResult)
    {
        std::shared_ptr<social_graph> pThis(thisWeakPtr.lock());
        if (pThis)
        {
            if (!socialListResult.err())
            {
                auto& socialList = socialListResult.payload();
                xsapi_internal_unordered_map<uint64_t, xbox_social_user> socialMap;
                for (auto& user : socialList)
                {
                    socialMap[user._Xbox_user_id_as_integer()] = user;
                }

                pThis->perform_diff(socialMap);
            }
            else
            {
                LOGS_ERROR_IF(social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error)
                    << "social_graph: refresh_graph call failed with error: " << socialListResult.err() << " " << socialListResult.err_message();
            }
        }
    });
}

void
social_graph::perform_diff(
    _In_ const xsapi_internal_unordered_map<uint64_t, xbox_social_user>& xboxSocialUsers
    )
{
    std::lock_guard<std::recursive_mutex> socialGraphStateLock(m_socialGraphStateMutex);
    {
        std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
        std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);
        m_perfTester.start_timer("set_state");
        if (m_userBuffer.inactive_buffer() == nullptr)
        {
            LOG_ERROR_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                "Diff cannot happening with null buffer"
            );
            return;
        }
        set_state(social_graph_state::diff);
        m_perfTester.stop_timer("set_state");
    }

    xsapi_internal_vector<xbox_social_user> usersAddedList;
    xsapi_internal_vector<uint64_t> usersRemovedList;
    xsapi_internal_vector<social_manager_presence_record> presenceChangeList;
    xsapi_internal_vector<xbox_social_user> socialRelationshipChangeList;
    xsapi_internal_vector<xbox_social_user> profileChangeList;

    for (auto& currentUserPair : xboxSocialUsers)
    {
        m_perfTester.start_timer("perform_diff: start");
        auto inactiveBufferUserGraph = m_userBuffer.inactive_buffer()->socialUserGraph;
        if (inactiveBufferUserGraph.find(currentUserPair.first) == inactiveBufferUserGraph.end())
        {
            usersAddedList.push_back(currentUserPair.second);
            continue;
        }

        auto previousUser = inactiveBufferUserGraph.at(currentUserPair.first).socialUser;
        change_list_enum didChange = previousUser ? xbox_social_user::_Compare(*previousUser, currentUserPair.second)
            : (change_list_enum::presence_change | change_list_enum::profile_change | change_list_enum::social_relationship_change);

        if ((didChange & change_list_enum::presence_change) == change_list_enum::presence_change)
        {
            presenceChangeList.push_back(currentUserPair.second.presence_record());
        }
        if ((didChange & change_list_enum::profile_change) == change_list_enum::profile_change)
        {
            profileChangeList.push_back(currentUserPair.second);
        }
        if ((didChange & change_list_enum::social_relationship_change) == change_list_enum::social_relationship_change)
        {
            socialRelationshipChangeList.push_back(currentUserPair.second);
        }
    }

    auto inactiveBufferUserGraph = m_userBuffer.inactive_buffer()->socialUserGraph;
    for (auto& previousUserPair : inactiveBufferUserGraph)
    {
        if (xboxSocialUsers.find(previousUserPair.first) == xboxSocialUsers.end() && 
            (previousUserPair.second.socialUser != nullptr && previousUserPair.second.socialUser->is_following_user()))
        {
            usersRemovedList.push_back(previousUserPair.first);
        }
    }

    if (usersAddedList.size() > 0)
    {
        m_unprocessedEventQueue.push(unprocessed_social_event_type::users_changed, usersAddedList);
    }
    if (usersRemovedList.size() > 0)
    {
        m_unprocessedEventQueue.push(unprocessed_social_event_type::users_removed, usersRemovedList);
    }
    if (presenceChangeList.size() > 0)
    {
        m_unprocessedEventQueue.push(unprocessed_social_event_type::presence_changed, presenceChangeList);
    }
    if (profileChangeList.size() > 0)
    {
        m_unprocessedEventQueue.push(unprocessed_social_event_type::profiles_changed, profileChangeList);
    }
    if (socialRelationshipChangeList.size() > 0)
    {
        m_unprocessedEventQueue.push(unprocessed_social_event_type::social_relationships_changed, socialRelationshipChangeList);
    }

    {
        std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
        std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);
        m_perfTester.start_timer("set_state normal");
        set_state(social_graph_state::normal);
        m_perfTester.stop_timer("set_state normal");
    }
}

uint32_t
social_graph::title_id()
{
    return m_xboxLiveContextImpl->application_config()->title_id();
}

change_struct
social_graph::do_work(
    _Inout_ xsapi_internal_vector<std::shared_ptr<social_event_internal>>& socialEvents
    )
{
    m_perfTester.start_timer("do_work");
    m_perfTester.start_timer("do_work locktime");
    std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);
    m_perfTester.stop_timer("do_work locktime");
    m_numEventsThisFrame = 0;
    change_struct changeStruct;
    changeStruct.socialUsers = nullptr;
    m_perfTester.start_timer("social_graph_state_check");
    if (m_socialGraphState == social_graph_state::normal && m_userBuffer.inactive_buffer() != nullptr && m_userBuffer.inactive_buffer()->socialUserEventQueue.empty())
    {
        m_perfTester.start_timer("user buffer swap");
        m_userBuffer.swap();
        m_perfTester.stop_timer("user buffer swap");
    }
    m_perfTester.stop_timer("social_graph_state_check");
    m_perfTester.start_timer("assign active buffer");
    if (m_userBuffer.active_buffer() != nullptr)
    {
        changeStruct.socialUsers = &m_userBuffer.active_buffer()->socialUserGraph;
    }
    m_perfTester.stop_timer("assign active buffer");
    m_perfTester.start_timer("!m_socialEventQueue.empty()");
    if (!m_socialEventQueue.empty() && m_socialGraphState == social_graph_state::normal)
    {
        m_perfTester.start_timer("do_work: social event push_back");
        socialEvents.reserve(socialEvents.size() + m_socialEventQueue.social_event_list().size());
        for (auto& evt : m_socialEventQueue.social_event_list())
        {
            socialEvents.push_back(evt);
        }
        m_socialEventQueue.clear();
        m_perfTester.stop_timer("do_work: social event push_back");
    }
    m_perfTester.stop_timer("!m_socialEventQueue.empty()");
    m_perfTester.stop_timer("do_work");
    m_perfTester.clear();
    return changeStruct;
}

void
social_graph::social_graph_timer_callback(
    _In_ const xsapi_internal_vector<xsapi_internal_string>& users,
    _In_ std::shared_ptr<call_buffer_timer_completion_context> completionContext
    )
{
    auto stdUsers = utils::std_string_vector_from_internal_string_vector(users);

    std::weak_ptr<social_graph> thisWeakPtr = shared_from_this();
    m_peoplehubService.get_social_graph(
        m_xboxLiveContextImpl->xbox_live_user_id(),
        m_detailLevel,
        users,
        m_backgroundAsyncQueue,
    [thisWeakPtr, users, completionContext](xbox_live_result<xsapi_internal_vector<xbox_social_user>> socialListResult)
    {
        try
        {
            std::shared_ptr<social_graph> pThis(thisWeakPtr.lock());
            if (pThis)
            {
                if (!socialListResult.err())
                {
                    pThis->m_unprocessedEventQueue.push(unprocessed_social_event_type::users_changed, socialListResult.payload(), completionContext);
                }
                else
                {
                    xsapi_internal_vector<xsapi_internal_string> xsapiStrVec;
                    for (auto user : users)
                    {
                        xsapiStrVec.push_back(user.c_str());
                    }
                    unprocessed_social_event evt(unprocessed_social_event_type::users_changed, xbox_live_result<void>(socialListResult.err(), socialListResult.err_message()), xsapiStrVec);
                    evt.set_completion_context(completionContext);
                    pThis->m_unprocessedEventQueue.push(evt);
                }
            }
        }
        catch (const std::exception& e)
        {
            LOGS_DEBUG_IF(social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::verbose) 
                << "Exception in social_graph_timer_callback " << e.what();
        }
        catch (...)
        {
            LOG_DEBUG_IF(social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::verbose, "Unknown std::exception in initialization");
        }
    });
}

void social_graph::schedule_social_graph_refresh()
{
#if UWP_API || TV_API || UNIT_TEST_SERVICES
    std::weak_ptr<social_graph> thisWeakPtr = shared_from_this();

    AsyncBlock* async = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
    async->queue = m_backgroundAsyncQueue;
    async->context = utils::store_weak_ptr(thisWeakPtr);
    async->callback = [](AsyncBlock* async) 
    {
        auto pThis = utils::get_shared_ptr<social_graph>(async->context);
        if (pThis)
        {
            pThis->schedule_social_graph_refresh();
        }
        xsapi_memory::mem_free(async);
    };
    BeginAsync(async, utils::store_weak_ptr(thisWeakPtr), nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        if (op == AsyncOp_DoWork)
        {
            try
            {
                auto pThis = utils::get_shared_ptr<social_graph>(data->context);
                if (pThis)
                {
                    pThis->refresh_graph();
                }
            }
            catch (const std::exception& e)
            {
                LOGS_DEBUG_IF(social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::verbose)
                    << "Exception in social_graph_refresh_callback " << e.what();
            }
            catch (...)
            {
                LOG_DEBUG_IF(
                    social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::verbose,
                    "Unknown std::exception in initialization"
                );
            }
            CompleteAsync(data->async, S_OK, 0);
            return E_PENDING;
        }
        return S_OK;
    });
    ScheduleAsync(async, (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(REFRESH_TIME_MIN).count());
#endif
}

void
social_graph::handle_device_presence_change(
    _In_ std::shared_ptr<device_presence_change_event_args_internal> devicePresenceChanged
    )
{
    uint64_t id = utils::internal_string_to_uint64(devicePresenceChanged->xbox_user_id());
    if (id == 0)
    {
        LOG_ERROR_IF(
            social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
            "Invalid user"
        );
        return;
    }

    m_unprocessedEventQueue.push(unprocessed_social_event(unprocessed_social_event_type::device_presence_changed, devicePresenceChanged));
}

void
social_graph::handle_title_presence_change(
    _In_ std::shared_ptr<xbox::services::presence::title_presence_change_event_args_internal> titlePresenceChanged
    )
{
    if (titlePresenceChanged->title_state() == title_presence_state::started)
    {
        xsapi_internal_vector<xsapi_internal_string> presenceVec(1, titlePresenceChanged->xbox_user_id());
        m_presenceRefreshTimer->fire(presenceVec);
    }
    else
    {
        unprocessed_social_event titlePresenceChangeEvent(unprocessed_social_event_type::title_presence_changed, titlePresenceChanged);
        m_unprocessedEventQueue.push(titlePresenceChangeEvent);
    }
}

void
social_graph::handle_social_relationship_change(
    _In_ std::shared_ptr<xbox::services::social::social_relationship_change_event_args_internal> socialRelationshipChanged
    )
{
    auto socialNotification = socialRelationshipChanged->social_notification();
    if (socialNotification == social_notification_type::added)
    {
        m_unprocessedEventQueue.push(unprocessed_social_event_type::users_added, socialRelationshipChanged->xbox_user_ids());
    }
    else if (socialNotification == social_notification_type::changed)
    {
        m_socialGraphRefreshTimer->fire(socialRelationshipChanged->xbox_user_ids());
    }
    else if (socialNotification == social_notification_type::removed)
    {
        xsapi_internal_vector<uint64_t> xboxUserIdsAsInt;
        xboxUserIdsAsInt.reserve(socialRelationshipChanged->xbox_user_ids().size());

        for (auto& xuid : socialRelationshipChanged->xbox_user_ids())
        {
            uint64_t id = utils::internal_string_to_uint64(xuid);
            if (id == 0)
            {
                LOG_ERROR_IF(
                    social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                    "Invalid user"
                );
                continue;
            }
            xboxUserIdsAsInt.push_back(id);
        }

        remove_users(xboxUserIdsAsInt);
    }
}

void
social_graph::handle_rta_subscription_error(
    _In_ xbox::services::real_time_activity::real_time_activity_subscription_error_event_args& rtaErrorEventArgs
    )
{
    LOGS_ERROR_IF(social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error)
        << "RTA subscription error occurred in social manager: " << rtaErrorEventArgs.err().message() << " " << rtaErrorEventArgs.err_message();
}

void
social_graph::handle_rta_connection_state_change(
    _In_ real_time_activity_connection_state rtaState
    )
{
    bool wasDisconnected = false;
    {
        std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
        std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);
        m_perfTester.start_timer("handle_rta_connection_state_change:disconnected_check");
        wasDisconnected = m_wasDisconnected;
        m_perfTester.stop_timer("handle_rta_connection_state_change:disconnected_check");
    }
    if(rtaState == real_time_activity_connection_state::disconnected)
    {
        {
            std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
            std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex); 
            m_perfTester.start_timer("handle_rta_connection_state_change: disconnected received");
            m_wasDisconnected = true;
            m_perfTester.stop_timer("handle_rta_connection_state_change: disconnected received");
        }
    }
    else if (wasDisconnected)
    {
        {
            std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
            std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);
            m_perfTester.start_timer("handle_rta_connection_state_change: disconnected check false");
            m_wasDisconnected = false;
            m_perfTester.stop_timer("handle_rta_connection_state_change: disconnected check false");
        }
        setup_rta_subscriptions(true);
    }

    _Trigger_rta_connection_state_change_event(rtaState);
}

void
social_graph::_Trigger_rta_connection_state_change_event(
    _In_ xbox::services::real_time_activity::real_time_activity_connection_state state
    )
{
    if (m_stateRTAFunction != nullptr)
    {
        m_stateRTAFunction(state);
    }
}

void
social_graph::presence_timer_callback(
    _In_ const xsapi_internal_vector<xsapi_internal_string>& users
    )
{
    if (users.empty())
    {
        return;
    }
    std::weak_ptr<social_graph> thisWeakPtr = shared_from_this();

    m_xboxLiveContextImpl->presence_service()->get_presence_for_multiple_users(
        users,
        xsapi_internal_vector<presence_device_type>(),
        xsapi_internal_vector<uint32_t>(),
        presence_detail_level::all,
        false,
        false,
        m_backgroundAsyncQueue,
        [thisWeakPtr](xbox_live_result<xsapi_internal_vector<std::shared_ptr<presence_record_internal>>> presenceRecordsResult)
    {
        std::shared_ptr<social_graph> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            if (!presenceRecordsResult.err())
            {
                std::lock_guard<std::recursive_mutex> socialGraphStateLock(pThis->m_socialGraphStateMutex);
                {
                    std::lock_guard<std::recursive_mutex> lock(pThis->m_socialGraphMutex);
                    std::lock_guard<std::recursive_mutex> priorityLock(pThis->m_socialGraphPriorityMutex);
                    pThis->m_perfTester.start_timer("social graph refresh state set");
                    if (pThis->m_userBuffer.inactive_buffer() == nullptr)
                    {
                        LOG_ERROR_IF(
                            social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                            "Cannot update presence when user buffer is null"
                        );
                        return;
                    }
                    pThis->set_state(social_graph_state::refresh);
                    pThis->m_perfTester.start_timer("social graph refresh state set");
                }

                auto presenceRecordReturnVec = presenceRecordsResult.payload();
                xsapi_internal_vector<social_manager_presence_record> socialManagerPresenceVec;
                socialManagerPresenceVec.reserve(presenceRecordReturnVec.size());
                for (auto& presenceRecord : presenceRecordReturnVec)
                {
                    socialManagerPresenceVec.push_back(social_manager_presence_record(presenceRecord));
                }

                std::vector<social_manager_presence_record> presenceRecordChanges;
                for (auto& record : socialManagerPresenceVec)
                {
                    auto previousRecordIter = pThis->m_userBuffer.inactive_buffer()->socialUserGraph.find(record._Xbox_user_id());
                    if (previousRecordIter == pThis->m_userBuffer.inactive_buffer()->socialUserGraph.end())
                    {
                        continue;
                    }

                    if (previousRecordIter->second.socialUser == nullptr)
                    {
                        continue;
                    }
                    auto& previousRecord = previousRecordIter->second.socialUser->presence_record();
                    if (previousRecord._Compare(record))
                    {
                        presenceRecordChanges.push_back(record);
                    }
                }

                pThis->m_unprocessedEventQueue.push(
                    unprocessed_social_event_type::presence_changed,
                    socialManagerPresenceVec
                    );

                {
                    std::lock_guard<std::recursive_mutex> lock(pThis->m_socialGraphMutex);
                    std::lock_guard<std::recursive_mutex> priorityLock(pThis->m_socialGraphPriorityMutex);
                    pThis->m_perfTester.start_timer("social graph refresh state set normal");
                    pThis->set_state(social_graph_state::normal);
                    pThis->m_perfTester.stop_timer("social graph refresh state set normal");
                }
            }
            else
            {
                LOG_ERROR_IF(
                    social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                    "social_graph: presence record update failed"
                );
            }
        }
    });
}

bool
social_graph::are_events_empty()
{
    std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
    std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);
    m_perfTester.start_timer("are_events_empty");
    auto result =  m_userBuffer.user_buffer_a().socialUserEventQueue.empty() && m_userBuffer.user_buffer_b().socialUserEventQueue.empty();
    m_perfTester.stop_timer("are_events_empty");
    return result;
}

void
social_graph::add_users(
    _In_ const xsapi_internal_vector<xsapi_internal_string>& users,
    _In_ xbox_live_callback<xbox_live_result<void>> callback
    )
{
    m_unprocessedEventQueue.push(unprocessed_social_event(unprocessed_social_event_type::users_added, users, callback));    // this is fine to be n-sized because it will generate 0 events
}

void
social_graph::remove_users(
    _In_ const xsapi_internal_vector<uint64_t>& users
    )
{
    m_unprocessedEventQueue.push(unprocessed_social_event_type::users_removed, users);
}

void
social_graph::schedule_presence_refresh()
{
#if UWP_API || TV_API || UNIT_TEST_SERVICES
    std::weak_ptr<social_graph> thisWeakPtr = shared_from_this();

    AsyncBlock* async = new (xsapi_memory::mem_alloc(sizeof(AsyncBlock))) AsyncBlock{};
    async->queue = m_backgroundAsyncQueue;
    async->context = utils::store_weak_ptr(thisWeakPtr);
    async->callback = [](AsyncBlock* async)
    {
        auto pThis = utils::get_shared_ptr<social_graph>(async->context);
        if (pThis && !pThis->m_shouldCancel)
        {
            pThis->schedule_presence_refresh();
        }
        xsapi_memory::mem_free(async);
    };
    BeginAsync(async, utils::store_weak_ptr(thisWeakPtr), nullptr, __FUNCTION__,
        [](AsyncOp op, const AsyncProviderData* data)
    {
        if (op == AsyncOp_DoWork)
        {
            auto pThis = utils::get_shared_ptr<social_graph>(data->context);
            if (pThis && !pThis->m_shouldCancel)
            {
                xsapi_internal_vector<xsapi_internal_string> userList;
                {
                    std::lock_guard<std::recursive_mutex> socialGraphStateLock(pThis->m_socialGraphStateMutex);

                    if (pThis->m_userBuffer.inactive_buffer() != nullptr)
                    {
                        {
                            std::lock_guard<std::recursive_mutex> lock(pThis->m_socialGraphMutex);
                            std::lock_guard<std::recursive_mutex> priorityLock(pThis->m_socialGraphPriorityMutex);
                            pThis->m_perfTester.start_timer("presence refresh state set");
                            pThis->set_state(social_graph_state::refresh);
                            pThis->m_perfTester.stop_timer("presence refresh state set");
                        }
                        userList.reserve(pThis->m_userBuffer.inactive_buffer()->socialUserGraph.size());
                        for (auto& user : pThis->m_userBuffer.inactive_buffer()->socialUserGraph)
                        {
                            if (user.second.socialUser != nullptr)
                            {
                                userList.push_back(utils::internal_string_from_string_t(user.second.socialUser->xbox_user_id()));
                            }
                        }

                        pThis->m_presencePollingTimer->fire(userList);

                        {
                            std::lock_guard<std::recursive_mutex> lock(pThis->m_socialGraphMutex);
                            std::lock_guard<std::recursive_mutex> priorityLock(pThis->m_socialGraphPriorityMutex);
                            pThis->m_perfTester.start_timer("presence refresh fire");
                            pThis->set_state(social_graph_state::normal);
                            pThis->m_perfTester.stop_timer("presence refresh fire");
                        }
                    }
                }
            }
            CompleteAsync(data->async, S_OK, 0);
        }
        return S_OK;
    });
    ScheduleAsync(async, (uint32_t)std::chrono::duration_cast<std::chrono::milliseconds>(TIME_PER_CALL_SEC).count());
#endif
}

void
social_graph::enable_rich_presence_polling(
    _In_ bool shouldEnablePolling
    )
{
    bool isPollingRichPresence;
    {
        std::lock_guard<std::recursive_mutex> lock(m_socialGraphMutex);
        std::lock_guard<std::recursive_mutex> priorityLock(m_socialGraphPriorityMutex);
        isPollingRichPresence = m_isPollingRichPresence;
        m_isPollingRichPresence = shouldEnablePolling;
    }

    if (shouldEnablePolling && !isPollingRichPresence)
    {
        {
            std::lock_guard<std::recursive_mutex> socialGraphStateLock(m_socialGraphStateMutex);
            m_shouldCancel = false;
        }
        schedule_presence_refresh();
    }
    else if(!shouldEnablePolling)
    {
        std::lock_guard<std::recursive_mutex> socialGraphStateLock(m_socialGraphStateMutex);
        m_shouldCancel = true;
    }
}

void social_graph::clear_debug_counters()
{
}

void social_graph::print_debug_info()
{
}

const uint32_t user_buffers_holder::EXTRA_USER_FREE_SPACE = 5;

user_buffers_holder::user_buffers_holder() : m_activeBuffer(nullptr), m_inactiveBuffer(nullptr)
{
}

user_buffers_holder::~user_buffers_holder()
{
    LOG_DEBUG_IF(get_xsapi_singleton(false) && social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::verbose, "destroying user buffer holder");

    if (m_userBufferA.buffer != nullptr)
    {
        xsapi_memory::mem_free(m_userBufferA.buffer);
    }

    if (m_userBufferB.buffer != nullptr)
    {
        xsapi_memory::mem_free(m_userBufferB.buffer);
    }
}

void
user_buffers_holder::initialize(
    _In_ const xsapi_internal_vector<xbox_social_user>& users
    )
{
    initialize_buffer(m_userBufferA, users);
    initialize_buffer(m_userBufferB, users);
    m_activeBuffer = &m_userBufferA;
    m_inactiveBuffer = &m_userBufferB;
}

user_buffer&
user_buffers_holder::user_buffer_a()
{
    return m_userBufferA;
}

user_buffer& user_buffers_holder::user_buffer_b()
{
    return m_userBufferB;
}

void
user_buffers_holder::initialize_buffer(
    _Inout_ user_buffer& userBuffer,
    _In_ const xsapi_internal_vector<xbox_social_user>& users,
    _In_ size_t freeSpaceRequired
    )
{
    auto usersSize = users.size();
    buffer_init(userBuffer, users, freeSpaceRequired);
    initialize_users_in_map(userBuffer, usersSize, 0);
}

void user_buffers_holder::buffer_init(
    _Inout_ user_buffer& userBuffer,
    _In_ const xsapi_internal_vector<xbox_social_user>& users,
    _In_ size_t freeSpaceRequired
    )
{
    userBuffer.freeData = xsapi_internal_queue<byte*>();
    size_t allocatedSize = 0;
    auto buffer = buffer_alloc(users.size(), allocatedSize, freeSpaceRequired);
    if (buffer == nullptr)
    {
        return; // return with error
    }

    userBuffer.buffer = buffer;

    auto usersSize = users.size();
    auto socialUserSize = sizeof(xbox_social_user);

    for (uint32_t i = 0; i < usersSize; ++i)
    {
        auto xboxSocialUser = reinterpret_cast<xbox_social_user*>(userBuffer.buffer + (i * socialUserSize));
        new (xboxSocialUser) xbox_social_user();

        *xboxSocialUser = users[i];
    }

    auto totalFreeSpace = EXTRA_USER_FREE_SPACE + freeSpaceRequired;
    auto startOffset = userBuffer.buffer + users.size() * socialUserSize;
    for (uint32_t i = 0; i < totalFreeSpace; ++i)
    {
        userBuffer.freeData.push(startOffset + i * socialUserSize);
    }
}

byte*
user_buffers_holder::buffer_alloc(
    _In_ size_t numUsers,
    _Out_ size_t& allocatedSize,
    _In_ size_t freeSpaceRequired
    )
{
    if (numUsers == 0 && freeSpaceRequired == 0)
    {
        allocatedSize = 0;
        return nullptr;
    }

    auto totalFreeSpace = EXTRA_USER_FREE_SPACE + freeSpaceRequired;  // gives some wiggle room with the alloc, 5 extra users can be added to graph before realloc

    size_t size = (numUsers + totalFreeSpace) * sizeof(xbox_social_user);
    auto buffer = static_cast<byte*>(xsapi_memory::mem_alloc(size));
    allocatedSize = size;
    return buffer;
}

void
user_buffers_holder::initialize_users_in_map(
    _Inout_ user_buffer& userBuffer,
    _In_ size_t numUsers,
    _In_ size_t bufferOffset
    )
{
    xsapi_internal_unordered_map<uint64_t, xbox_social_user_context>& socialUserGraph = userBuffer.socialUserGraph;
    auto buffer = userBuffer.buffer + bufferOffset;
    for (uint32_t i = 0; i < numUsers; ++i)
    {
        auto userPtr = (buffer + i * sizeof(xbox_social_user));
        xbox_social_user* socialUser = reinterpret_cast<xbox_social_user*>(userPtr);

        auto userIter = socialUserGraph.find(socialUser->_Xbox_user_id_as_integer());
        if (userIter == socialUserGraph.end())
        {
            xbox_social_user_context userContext;
            userContext.refCount = 1;
            userContext.socialUser = socialUser;

            socialUserGraph[socialUser->_Xbox_user_id_as_integer()] = userContext;
        }
        else
        {
            userIter->second.socialUser = socialUser;
        }
    }
}

void
user_buffers_holder::add_users_to_buffer(
    _In_ const xsapi_internal_vector<xbox_social_user>& users,
    _Inout_ user_buffer& userBufferInactive,
    _In_ size_t finalSize
    )
{
    auto totalSizeNeeded = __max(finalSize, users.size());
    if (totalSizeNeeded > userBufferInactive.freeData.size())
    {
        uint32_t size = 0;
        for (auto& user : userBufferInactive.socialUserGraph)
        {
            if (user.second.socialUser != nullptr)
            {
                ++size;
            }
        }

        xsapi_internal_vector<xbox_social_user> socialVec(size);
        if (size > 0)
        {
#if _WIN32
            memcpy_s(&socialVec[0], socialVec.size() * sizeof(xbox_social_user), &userBufferInactive.buffer[0], size * sizeof(xbox_social_user));
#else
            memcpy(&socialVec[0], &userBufferInactive.buffer[0], size * sizeof(xbox_social_user));
#endif
        }
        xsapi_memory::mem_free(userBufferInactive.buffer);
        initialize_buffer(userBufferInactive, socialVec, totalSizeNeeded);
    }

    for (auto& user : users)
    {
        auto freeData = userBufferInactive.freeData.front();
        xbox_social_user* xboxSocialUser = reinterpret_cast<xbox_social_user*>(freeData);
        userBufferInactive.freeData.pop();
        new (freeData) xbox_social_user();

        *xboxSocialUser = user;
        initialize_users_in_map(userBufferInactive, 1, freeData - userBufferInactive.buffer);
    }
}

void
user_buffers_holder::remove_users_from_buffer(
    _In_ const xsapi_internal_vector<uint64_t>& users,
    _Inout_ user_buffer& userBufferInactive
    )
{
    for (auto user : users)
    {
        auto xboxSocialUserContextIter = userBufferInactive.socialUserGraph.find(user);
        if (xboxSocialUserContextIter != userBufferInactive.socialUserGraph.end())
        {
            auto userPtr = userBufferInactive.socialUserGraph[user].socialUser;
            userBufferInactive.freeData.push(reinterpret_cast<byte*>(userPtr));
            userBufferInactive.socialUserGraph.erase(xboxSocialUserContextIter);
        }
        else
        {
            LOG_ERROR_IF(
                social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::error,
                "user_buffers_holder: user not found in buffer"
            );
        }
    }
}

void
user_buffers_holder::swap()
{
    if (m_activeBuffer == &m_userBufferA)
    {
        m_activeBuffer = &m_userBufferB;
        m_inactiveBuffer = &m_userBufferA;
    }
    else
    {
        m_activeBuffer = &m_userBufferA;
        m_inactiveBuffer = &m_userBufferB;
    }
}

user_buffer* user_buffers_holder::active_buffer()
{
    return m_activeBuffer;
}

user_buffer*
user_buffers_holder::inactive_buffer()
{
    return m_inactiveBuffer;
}

void
user_buffers_holder::add_event(
    _In_ const unprocessed_social_event& internalSocialEvent
    )
{
    m_activeBuffer->socialUserEventQueue.push(internalSocialEvent);
}

event_queue::event_queue() :
    m_lastKnownSize(0),
    m_eventState(event_state::clear)
{
}

event_queue::event_queue(
    _In_ const xbox_live_user_t& user_t
    ) :
    m_user(std::move(user_t)),
    m_lastKnownSize(0),
    m_eventState(event_state::clear)
{
}

const xsapi_internal_vector<std::shared_ptr<social_event_internal>>&
event_queue::social_event_list()
{
    std::lock_guard<std::mutex> lock(m_eventGraphMutex.get());
    m_eventState = event_state::read;
    return m_socialEventList;
}

void
event_queue::push(
    _In_ const unprocessed_social_event& socialEvent,
    _In_ xbox_live_user_t user,
    _In_ social_event_type socialEventType,
    _In_ xbox_live_result<void> error
    )
{
    if (socialEventType == social_event_type::unknown)
    {
        return;
    }

    xsapi_internal_vector<xbox_user_id_container> usersAffected;
    usersAffected.reserve(socialEvent.users_affected_as_string_vec().size());
    for (auto& affectedUser : socialEvent.users_affected_as_string_vec())
    {
        usersAffected.push_back(utils::string_t_from_internal_string(affectedUser).data());
    }

    std::lock_guard<std::mutex> lock(m_eventGraphMutex.get());

    m_socialEventList.push_back(xsapi_allocate_shared<social_event_internal>(
            user, socialEventType, usersAffected, nullptr, error.err(), error.err_message().data()
            ));

    m_eventState = event_state::ready_to_read;
}

void
event_queue::clear()
{
    std::lock_guard<std::mutex> lock(m_eventGraphMutex.get());
    m_socialEventList.clear();
    m_eventState = event_state::clear;
}

bool
event_queue::empty()
{
    std::lock_guard<std::mutex> lock(m_eventGraphMutex.get());
    return m_socialEventList.empty();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END

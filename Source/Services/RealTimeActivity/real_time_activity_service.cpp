// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "Utils.h"
#include "user_context.h"
#include "xbox_system_factory.h"
#include "xsapi/real_time_activity.h"
#include "web_socket_connection.h"
#include "web_socket_connection_state.h"
#include "web_socket_client.h"
#include "utils.h"
#include "xbox_live_app_config_internal.h"
using namespace pplx;

NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_BEGIN

real_time_activity_service::real_time_activity_service(
    _In_ std::shared_ptr<xbox::services::user_context> userContext,
    _In_ std::shared_ptr<xbox::services::xbox_live_context_settings> xboxLiveContextSettings,
    _In_ std::shared_ptr<xbox::services::xbox_live_app_config> appConfig
    ) :
    m_userContext(std::move(userContext)),
    m_xboxLiveContextSettings(std::move(xboxLiveContextSettings)),
    m_appConfig(std::move(appConfig)),
    m_sequenceNumber(0),
    m_subscriptionErrorHandlerCounter(0),
    m_connectionStateChangeHandlerCounter(0),
    m_resyncHandlerCounter(0),
    m_connectionState(real_time_activity_connection_state::disconnected)
{
}

real_time_activity_service::~real_time_activity_service()
{
    if (m_userContext->caller_context_type() == caller_context_type::title)
    {
        deactivate();
    }
}

void
real_time_activity_service::activate()
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);
    auto& xuid = m_userContext->xbox_user_id();
    int activationCount = 0;
    {
        auto xsapiSingleton = get_xsapi_singleton();
        std::lock_guard<std::mutex> guard(xsapiSingleton->m_rtaActivationCounterLock);
        if (m_webSocketConnection == nullptr)
        {
            activationCount = ++xsapiSingleton->m_rtaActiveSocketCountPerUser[xuid];

            LOGS_DEBUG << "websocket count is at " << xsapiSingleton->m_rtaActiveSocketCountPerUser[xuid] << " for user " << m_userContext->xbox_user_id();
        }

        if (m_userContext->caller_context_type() == caller_context_type::multiplayer_manager ||
            m_userContext->caller_context_type() == caller_context_type::social_manager)
        {
            ++xsapiSingleton->m_rtaActiveManagersByUser[xuid];
            LOGS_DEBUG << "websocket manager count is at " << xsapiSingleton->m_rtaActiveManagersByUser[xuid] << " for user " << m_userContext->xbox_user_id();
        }
    }

    if (activationCount > MAXIMUM_WEBSOCKETS_ACTIVATIONS_ALLOWED_PER_USER)
    {
        auto appConfig = xbox::services::xbox_live_app_config_internal::get_app_config_singleton();
        if (utils::str_icmp(appConfig->sandbox(), "RETAIL") != 0)
        {
            bool disableAsserts = appConfig->is_disable_asserts_for_maximum_number_of_websockets_activated();
            if (!disableAsserts)
            {
#if UNIT_TEST_SERVICES
                auto xsapiSingleton = get_xsapi_singleton();
                std::lock_guard<std::mutex> guard(xsapiSingleton->m_rtaActivationCounterLock);
                --xsapiSingleton->m_rtaActiveSocketCountPerUser[m_userContext->xbox_user_id()];
#endif
                std::stringstream msg;
                LOGS_ERROR << "You've currently activated  " << activationCount << " websockets.";
                LOGS_ERROR << "We recommend you don't activate more than " << MAXIMUM_WEBSOCKETS_ACTIVATIONS_ALLOWED_PER_USER << " websockets";
                LOG_ERROR("You can temporarily disable the assert by calling");
                LOG_ERROR("xboxLiveContext->settings()->disable_asserts_for_maximum_number_of_websockets_activated()");
                LOG_ERROR("however the issue must be addressed before certification.");

                XSAPI_ASSERT(false);
            }
        }
    }

    if (m_webSocketConnection == nullptr)
    {
        std::weak_ptr<real_time_activity_service> thisWeakPtr = shared_from_this();

#if TV_API
        m_rtaShutdownToken =
            Windows::ApplicationModel::Core::CoreApplication::Suspending += ref new Windows::Foundation::EventHandler<Windows::ApplicationModel::SuspendingEventArgs^>
            ([thisWeakPtr](_In_opt_ Platform::Object^ sender, _In_ Windows::ApplicationModel::SuspendingEventArgs^ eventArgs)
        {
            std::shared_ptr<real_time_activity_service> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->deactivate();
            }
        });
#elif UWP_API
        m_rtaShutdownToken =
            Windows::ApplicationModel::Core::CoreApplication::Exiting += ref new Windows::Foundation::EventHandler<Platform::Object^>
            ([thisWeakPtr](_In_opt_ Platform::Object^ sender, _In_ Platform::Object^ eventArgs)
        {
            std::shared_ptr<real_time_activity_service> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->deactivate();
            }
        });
#endif

        xsapi_internal_stringstream endpoint;
        endpoint << utils::create_xboxlive_endpoint("rta", xbox_live_app_config_internal::get_app_config_singleton(), "wss");
        endpoint << "/connect";

        m_webSocketConnection = std::make_shared<web_socket_connection>(
            m_userContext,
            endpoint.str(),
            "rta.xboxlive.com.V2",
            m_xboxLiveContextSettings
            );

        // We will reset these event handler on destructor, so it's safe to pass in 'this' here.
        m_webSocketConnection->set_connection_state_change_handler([thisWeakPtr](web_socket_connection_state oldState, web_socket_connection_state newState)
        {
            std::shared_ptr<real_time_activity_service> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->on_socket_connection_state_change(oldState, newState);
            }
        });

        m_webSocketConnection->set_received_handler([thisWeakPtr](xsapi_internal_string message)
        {
            std::shared_ptr<real_time_activity_service> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->on_socket_message_received(message);
            }
        });

        m_webSocketConnection->ensure_connected();
    }
}

void
real_time_activity_service::deactivate()
{
    std::shared_ptr<xsapi_singleton> xsapiSingleton = get_xsapi_singleton(false);
    if (xsapiSingleton != nullptr) // skip this if process is shutting down
    {
        std::lock_guard<std::mutex> guard(xsapiSingleton->m_rtaActivationCounterLock);
        auto& xuid = m_userContext->xbox_user_id();
        if (m_userContext->caller_context_type() == caller_context_type::title)
        {
            if (m_webSocketConnection != nullptr && xsapiSingleton->m_rtaActiveSocketCountPerUser[xuid] > 0)
            {
                --xsapiSingleton->m_rtaActiveSocketCountPerUser[xuid];
            }
        }
        else if(xsapiSingleton->m_rtaActiveManagersByUser[xuid] != 0)
        {
            auto counter = --xsapiSingleton->m_rtaActiveManagersByUser[xuid];
            if (counter > 0 )
            {
                // Since the Managers share the RTA service, only close the socket on the last deactivate() call for that user.
                return;
            }
            --xsapiSingleton->m_rtaActiveSocketCountPerUser[xuid];
            LOGS_DEBUG << "websocket count is at " << xsapiSingleton->m_rtaActiveSocketCountPerUser[xuid] << " for user " << xuid;
            xsapiSingleton->m_rtaActiveManagersByUser.erase(xuid);
        }
         
        if (xsapiSingleton->m_rtaActiveSocketCountPerUser[xuid] == 0)
        {
            xsapiSingleton->m_rtaActiveSocketCountPerUser.erase(xuid);
        }
    }

    try
    {
#if TV_API
        Windows::ApplicationModel::Core::CoreApplication::Suspending -= m_rtaShutdownToken;
#elif UWP_API
        Windows::ApplicationModel::Core::CoreApplication::Exiting -= m_rtaShutdownToken;
#endif
    }
    catch (...)
    {
        LOG_ERROR("Exception on unregistering CoreApplication events!");
    }

    _Close_websocket();

    // _Close_websocket has it's own locking inside, don't include in the next lock
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);
        m_connectionStateChangeHandler.clear();
        m_connectionStateChangeHandlerCounter = 0;
        m_subscriptionErrorHandler.clear();
        m_subscriptionErrorHandlerCounter = 0;
    }

}

function_context
real_time_activity_service::add_connection_state_change_handler(
    _In_ std::function<void(real_time_activity_connection_state)> handler
    )
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);
    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_connectionStateChangeHandlerCounter;
        m_connectionStateChangeHandler[m_connectionStateChangeHandlerCounter] = std::move(handler);

        // Since you could have activated the service already for this context, trigger a state changed event.
        if (m_connectionState != disconnected)
        {
            trigger_connection_state_changed_event(m_connectionState);
        }
    }

    return context;
}

void
real_time_activity_service::remove_connection_state_change_handler(
    _In_ function_context remove
    )
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);
    m_connectionStateChangeHandler.erase(remove);
}

function_context
real_time_activity_service::add_subscription_error_handler(
    _In_ std::function<void(const real_time_activity_subscription_error_event_args&)> handler
    )
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);
    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_subscriptionErrorHandlerCounter;
        m_subscriptionErrorHandler[m_subscriptionErrorHandlerCounter] = std::move(handler);
    }

    return context;
}

void
real_time_activity_service::remove_subscription_error_handler(
    _In_ function_context remove
    )
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);
    m_subscriptionErrorHandler.erase(remove);
}

void
real_time_activity_service::trigger_resync_event()
{
    std::unordered_map<function_context, std::function<void()>> resyncHandlerCopy;
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);
        resyncHandlerCopy = m_resyncHandler;
    }

    for (auto& resyncHandler : resyncHandlerCopy)
    {
        XSAPI_ASSERT(resyncHandler.second != nullptr);
        if (resyncHandler.second != nullptr)
        {
            try
            {
                resyncHandler.second();
            }
            catch (...)
            {
            }
        }
    }
}

function_context
real_time_activity_service::add_resync_handler(
    _In_ std::function<void()> handler
    )
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);
    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_resyncHandlerCounter;
        m_resyncHandler[m_resyncHandlerCounter] = std::move(handler);
    }

    return context;
}

void
real_time_activity_service::remove_resync_handler(
    _In_ function_context remove
    )
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);
    m_resyncHandler.erase(remove);
}

void
real_time_activity_service::_Trigger_subscription_error(
    real_time_activity_subscription_error_event_args args
    )
{
    std::unordered_map<function_context, std::function<void(real_time_activity_subscription_error_event_args)>> subscriptionErrorHandlerCopy;
    LOGS_DEBUG << "RTA subscription error occurred";
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);
        subscriptionErrorHandlerCopy = m_subscriptionErrorHandler;
    }

    for (auto& subHandler : subscriptionErrorHandlerCopy)
    {
        XSAPI_ASSERT(subHandler.second != nullptr);
        if (subHandler.second != nullptr)
        {
            try
            {
                subHandler.second(args);
            }
            catch (...)
            {
            }
        }
    }
}

void real_time_activity_service::clear_all_subscriptions()
{
    for (auto& subscriptionPair : m_pendingResponseSubscriptions)
    {
        auto subscription = subscriptionPair.second;
        subscription->_Set_state(real_time_activity_subscription_state::closed);
    }
    m_pendingResponseSubscriptions.clear();

    for (auto& subscriptionPair : m_subscriptions)
    {
        auto subscription = subscriptionPair.second;
        subscription->_Set_state(real_time_activity_subscription_state::closed);
    }
    m_subscriptions.clear();

    for (auto& subscriptionPair : m_pendingUnsubscriptions)
    {
        auto subscription = subscriptionPair.second;
        subscription->_Set_state(real_time_activity_subscription_state::closed);
    }
    m_pendingUnsubscriptions.clear();

    for (auto& subscription : m_pendingSubmission)
    {
        subscription->_Set_state(real_time_activity_subscription_state::closed);
    }
    m_pendingSubmission.clear();
}

void
real_time_activity_service::on_socket_connection_state_change(
    _In_ web_socket_connection_state oldState,
    _In_ web_socket_connection_state newState
    )
{
    if (oldState == newState) return;

    if (web_socket_connection_state::activated == newState) return;
    
    std::lock_guard<std::recursive_mutex> lock(m_lock);
    {
        if (newState == web_socket_connection_state::disconnected)
        {
            m_connectionState = real_time_activity_connection_state::disconnected;

            clear_all_subscriptions();
            trigger_connection_state_changed_event(real_time_activity_connection_state::disconnected);
        }

        // On connecting, set subscriptions state accordingly.
        if (newState == web_socket_connection_state::connecting)
        {
            m_connectionState = real_time_activity_connection_state::connecting;
            for (auto& subscriptionPair : m_subscriptions)
            {
                auto subscription = subscriptionPair.second;
                subscription->_Set_state(real_time_activity_subscription_state::pending_subscribe);
                m_pendingSubmission.push_back(subscription);
            }
            m_subscriptions.clear();

            for (auto& subscriptionPair : m_pendingResponseSubscriptions)
            {
                auto subscription = subscriptionPair.second;
                subscription->_Set_state(real_time_activity_subscription_state::pending_subscribe);
                m_pendingSubmission.push_back(subscription);
            }
            m_pendingResponseSubscriptions.clear();

            // clear out pending unsubscriptions, as it will be reset by service.
            for (auto& subscriptionPair : m_pendingUnsubscriptions)
            {
                auto subscription = subscriptionPair.second;
                subscription->_Set_state(real_time_activity_subscription_state::closed);
            }
            m_pendingUnsubscriptions.clear();

            trigger_connection_state_changed_event(real_time_activity_connection_state::connecting);
        }

        // socket reconnected, re-subscribe everything
        if (newState == web_socket_connection_state::connected)
        {
            m_connectionState = real_time_activity_connection_state::connected;
            submit_subscriptions();
            trigger_connection_state_changed_event(real_time_activity_connection_state::connected);
        }
    }
}

void
real_time_activity_service::trigger_connection_state_changed_event(
    _In_ real_time_activity_connection_state connectionState
    )
{
    std::unordered_map<function_context, std::function<void(real_time_activity_connection_state)>> connectionStateChangeHandlers;
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);
        connectionStateChangeHandlers = m_connectionStateChangeHandler;
    }

    for (auto& connectionHandler : connectionStateChangeHandlers)
    {
        XSAPI_ASSERT(connectionHandler.second != nullptr);
        if (connectionHandler.second != nullptr)
        {
            try
            {
                connectionHandler.second(connectionState);
            }
            catch (...)
            {
            }
        }
    }
}

void
real_time_activity_service::on_socket_message_received(
    _In_ const xsapi_internal_string& message
    )
{
    auto msgJson = web::json::value::parse(utils::string_t_from_internal_string(message));
    real_time_activity_message_type messageType = static_cast<real_time_activity_message_type>(msgJson[0].as_integer());

    switch (messageType)
    {
    case real_time_activity_message_type::subscribe:
        complete_subscribe(msgJson);
        break;
    case real_time_activity_message_type::unsubscribe:
        complete_unsubscribe(msgJson);
        break;
    case real_time_activity_message_type::change_event:
        handle_change_event(msgJson);
        break;
    case real_time_activity_message_type::resync:
        trigger_resync_event();
        break;
    default:
        throw std::runtime_error("Unexpected websocket message");
        break;
    }
}

void
real_time_activity_service::handle_change_event(
    _In_ web::json::value& message
    )
{
    // response format:
    //[<API_ID>, <SUB_ID>, <DATA>]
    int subscriptionId = message[1].as_integer();
    const auto& data = message[2];

    std::shared_ptr<real_time_activity_subscription> subscription;
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);
        auto iter = m_subscriptions.find(subscriptionId);
        if (iter != m_subscriptions.end())
        {
            subscription = iter->second;
        }
    }

    if (subscription != nullptr)
    {
        subscription->on_event_received(data);
    }
}

void
real_time_activity_service::complete_subscribe(
    _In_ web::json::value& message
    )
{
    // subscribe response format:
    //  [<API_ID>, <SEQUENCE_N>, <CODE_N>, <SUB_ID>, <DATA>]
    int32_t sequenceNum = message[1].as_integer();
    int32_t code = message[2].as_integer();
    std::shared_ptr<real_time_activity_subscription> subscription;
    {
        std::lock_guard<std::recursive_mutex> guard(m_lock);
        auto iter = m_pendingResponseSubscriptions.find(sequenceNum);
        if (iter != m_pendingResponseSubscriptions.end())
        {
            subscription = iter->second;
            m_pendingResponseSubscriptions.erase(iter);

        }
    }

    if (subscription != nullptr)
    {
        if (code == 0)
        {
            int subscriptionId = message[3].as_integer();
            const auto& data = message[4];

            {
                std::lock_guard<std::recursive_mutex> guard(m_lock);
                m_subscriptions[subscriptionId] = subscription;
            }

            subscription->on_subscription_created(subscriptionId, data);
        }
        else
        {
            auto xboxLiveErrCode = convert_rta_error_code_to_xbox_live_error_code(code);
            subscription->_Set_state(real_time_activity_subscription_state::closed);

            std::string errorStr = utility::conversions::to_utf8string(message[3].as_string());
            _Trigger_subscription_error(
                real_time_activity_subscription_error_event_args(
                    *subscription,
                    xboxLiveErrCode,
                    errorStr
                    )
                );
        }
    }
    else
    {
        LOG_ERROR("No subscription found that matches received message");
    }
}

void
real_time_activity_service::complete_unsubscribe(
    _In_ web::json::value& message
    )
{
    // response format:
    // [<API_ID>, <SEQUENCE_N>, <CODE_N>]
    int sequenceNum = message[1].as_integer();

    std::shared_ptr<real_time_activity_subscription> subscription;
    {
        std::lock_guard<std::recursive_mutex> guard(m_lock);
        auto iter = m_pendingUnsubscriptions.find(sequenceNum);
        if (iter != m_pendingUnsubscriptions.end())
        {
            subscription = iter->second;
            m_pendingUnsubscriptions.erase(iter);

            subscription->_Set_state(real_time_activity_subscription_state::closed);
        }
    }
}

xbox_live_result<void>
real_time_activity_service::_Add_subscription(
    _In_ std::shared_ptr<real_time_activity_subscription> subscription
    )
{
    if (subscription == nullptr)
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "subscription is null");
    }

    std::lock_guard<std::recursive_mutex> guard(m_lock);
    if ((m_webSocketConnection == nullptr || (m_webSocketConnection != nullptr && m_webSocketConnection->state() == web_socket_connection_state::disconnected)) &&
        m_connectionState == real_time_activity_connection_state::disconnected)
    {
        return xbox_live_result<void>(
            xbox_live_error_code::invalid_argument,
            "The websocket has been deactivated. Call activate to reconnect."
            );
    }

    subscription->_Set_state(real_time_activity_subscription_state::pending_subscribe);
    m_pendingSubmission.push_back(subscription);
    if (m_connectionState == real_time_activity_connection_state::connected)
    {
        submit_subscriptions();
    }

    return xbox_live_result<void>();
}

void
real_time_activity_service::submit_subscriptions()
{
    while (m_webSocketConnection != nullptr && !m_pendingSubmission.empty())
    {
        auto subscription = m_pendingSubmission.back();
        m_pendingSubmission.pop_back();
        int sequenceNumber = utils::interlocked_increment(m_sequenceNumber);
        m_pendingResponseSubscriptions[sequenceNumber] = subscription;

        web::json::value request;
        request[0] = static_cast<uint32_t>(real_time_activity_message_type::subscribe);
        request[1] = sequenceNumber;
        request[2] = web::json::value(subscription->resource_uri());

        m_webSocketConnection->send(utils::internal_string_from_string_t(request.serialize()));
    }
}

xbox_live_result<void>
real_time_activity_service::_Remove_subscription(
    _In_ std::shared_ptr<real_time_activity_subscription> subscription
    )
{
    if (subscription == nullptr)
    {
        return xbox_live_result<void>(
            xbox_live_error_code::invalid_argument,
            "subscription is null"
            );
    }

    std::lock_guard<std::recursive_mutex> guard(m_lock);
    auto subscriptionId = subscription->subscription_id();

    if (subscription->state() == real_time_activity_subscription_state::subscribed)
    {
        auto iter = m_subscriptions.find(subscriptionId);
        if (iter != m_subscriptions.end())
        {
            auto subscriptionIter = iter->second;
            m_subscriptions.erase(iter);

            int sequenceNumber = utils::interlocked_increment(m_sequenceNumber);
            subscriptionIter->_Set_state(real_time_activity_subscription_state::pending_unsubscribe);
            m_pendingUnsubscriptions[sequenceNumber] = subscriptionIter;

            web::json::value request;
            request[0] = static_cast<uint32_t>(real_time_activity_message_type::unsubscribe);
            request[1] = sequenceNumber;
            request[2] = subscriptionId;

            m_webSocketConnection->send(utils::internal_string_from_string_t(request.serialize()));
        }
    }
    else if(subscription->state() == real_time_activity_subscription_state::pending_subscribe)
    {
        std::vector<std::shared_ptr<real_time_activity_subscription>>::iterator it = m_pendingSubmission.begin();
        bool found = false;
        for (it; it != m_pendingSubmission.end(); ++it)
        {
            auto pendingSubmission = *it;
            if (pendingSubmission->m_guid == subscription->m_guid)
            {
                found = true;
                m_pendingSubmission.erase(it);
                break;
            }
        }

        if (!found)
        {
            std::map<uint32_t, std::shared_ptr<real_time_activity_subscription>>::iterator responseIt = m_pendingResponseSubscriptions.begin();
            for (responseIt; responseIt != m_pendingResponseSubscriptions.end(); ++responseIt)
            {
                auto pendingResponse = *responseIt;
                if (pendingResponse.second->m_guid == subscription->m_guid)
                {
                    m_pendingResponseSubscriptions.erase(responseIt);
                    break;
                }
            }
        }

        subscription->_Set_state(real_time_activity_subscription_state::closed);
    }
    return xbox_live_result<void>();
}

void
real_time_activity_service::_Close_websocket()
{
    std::shared_ptr<xbox::services::web_socket_connection> socketToClean;
    {
        std::lock_guard<std::recursive_mutex> lock(m_lock);
        // RTA service will cleanup subscription on websocket close, no need 
        // to send unsubscribe msg on websocket closing. Just to clear all local
        // stored subscriptions
        clear_all_subscriptions();

        socketToClean = m_webSocketConnection;
        m_webSocketConnection = nullptr;
        m_connectionState = real_time_activity_connection_state::disconnected;
    }

    if (socketToClean != nullptr)
    {
        socketToClean->set_received_handler(nullptr);
        socketToClean->close();
        socketToClean->set_connection_state_change_handler(nullptr);
    }
}

std::error_code
real_time_activity_service::convert_rta_error_code_to_xbox_live_error_code(
    _In_ int32_t rtaErrorCode
    )
{
    switch (rtaErrorCode)
    {
        case 0: return xbox_live_error_code::no_error;
        case 1: return xbox_live_error_code::rta_subscription_limit_reached;
        case 2: return xbox_live_error_code::rta_access_denied;
        default: return xbox_live_error_code::rta_generic_error;
    }
}

std::unordered_map<xsapi_internal_string, uint32_t> 
real_time_activity_service::_Rta_activation_map()
{
    auto xsapiSingleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> guard(xsapiSingleton->m_rtaActivationCounterLock);
    return xsapiSingleton->m_rtaActiveSocketCountPerUser;
}

std::unordered_map<xsapi_internal_string, uint32_t>
real_time_activity_service::_Rta_manager_activation_map()
{
    auto xsapiSingleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> guard(xsapiSingleton->m_rtaActivationCounterLock);
    return xsapiSingleton->m_rtaActiveManagersByUser;
}


NAMESPACE_MICROSOFT_XBOX_SERVICES_RTA_CPP_END
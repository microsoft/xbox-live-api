// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xbox_system_factory.h"
#include "xsapi/multiplayer.h"
#include "multiplayer_internal.h"

using namespace xbox::services::multiplayer;
using namespace xbox::services::system;
using namespace xbox::services;
using namespace pplx;

multiplayer_service_impl::multiplayer_service_impl(
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> rtaService
    ) :
    m_realTimeActivityService(rtaService),
    m_subscriptionEnabled(false),
    m_multiplayerSubscriptionLostEventHandlerCounter(0),
    m_sessionChangeEventHandlerCounter(0)
{
}

multiplayer_service_impl::~multiplayer_service_impl()
{
    disable_multiplayer_subscriptions();
    m_sessionChangeEventHandler.clear();
}

task<xbox_live_result<string_t>>
multiplayer_service_impl::ensure_multiplayer_subscription()
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock.get());

    RETURN_TASK_CPP_INVALIDARGUMENT_IF(m_realTimeActivityService == nullptr, string_t, "real_time_activity_service not initialized");

    if (m_subscription == nullptr)
    {
        std::weak_ptr<multiplayer_service_impl> thisWeakPtr = shared_from_this();
        m_subscription = xbox_system_factory::get_factory()->create_multiplayer_subscription(
            ([thisWeakPtr](const multiplayer_session_change_event_args& eventArgs)
            {
                std::shared_ptr<multiplayer_service_impl> pThis(thisWeakPtr.lock());
                if (pThis != nullptr)
                {
                    pThis->multiplayer_session_changed(eventArgs);
                }
            }),
            ([thisWeakPtr]()
            {
                std::shared_ptr<multiplayer_service_impl> pThis(thisWeakPtr.lock());
                if (pThis != nullptr)
                {
                    pThis->multiplayer_subscription_lost();
                }
            }),
            ([thisWeakPtr](const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args& eventArgs)
            {
                std::shared_ptr<multiplayer_service_impl> pThis(thisWeakPtr.lock());
                if (pThis != nullptr)
                {
                    pThis->m_realTimeActivityService->_Trigger_subscription_error(eventArgs);
                }
            })
            );

        auto subscriptionResult = m_realTimeActivityService->_Add_subscription(
            m_subscription
            );

        if (subscriptionResult.err())
        {
            m_subscription = nullptr;
            return pplx::task_from_result(xbox_live_result<string_t>(xbox_live_error_code::runtime_error, "Failed to create multiplayer subscription, please make sure real_time_activity_service::activate() is called and connection is connected"));
        }
    }

    return create_task(m_subscription->task);
}

void 
multiplayer_service_impl::multiplayer_session_changed(
    _In_ const multiplayer_session_change_event_args& eventArgs
    )
{
    std::unordered_map<uint32_t, std::function<void(const multiplayer_session_change_event_args&)>> sessionChangeCopy;
    {
        std::lock_guard<std::mutex> lock(m_subscriptionLock.get());
        sessionChangeCopy = m_sessionChangeEventHandler;
    }

    for(auto& handler : sessionChangeCopy)
    {
        XSAPI_ASSERT(handler.second != nullptr);
        LOG_ERROR_IF(handler.second == nullptr, "multiplayer_session_changed handle is null");
        if (handler.second != nullptr)
        {
            try
            {
                handler.second(eventArgs);
            }
            catch (...)
            {
                LOG_ERROR("multiplayer_session_changed call threw an exception");
            }
        }
    }
}

void
multiplayer_service_impl::multiplayer_subscription_lost()
{
    {
        std::lock_guard<std::mutex> lock(m_subscriptionEnabledLock.get());
        m_subscriptionEnabled = false;
    }

    std::unordered_map<uint32_t, std::function<void()>> multiplayerSubscriptionLostCopy;
    {
        std::lock_guard<std::mutex> lock(m_subscriptionLock.get());

        if (m_subscription)
        {
            auto subscription = m_subscription;
            auto rta = m_realTimeActivityService;
            pplx::create_task(
                [subscription, rta]()
            {
                rta->_Remove_subscription(subscription);
            }
            );
            m_subscription = nullptr;
        }

        multiplayerSubscriptionLostCopy = m_multiplayerSubscriptionLostEventHandler;
    }

    for(auto& handler : multiplayerSubscriptionLostCopy)
    {
        XSAPI_ASSERT(handler.second != nullptr);
        if (handler.second != nullptr)
        {
            try
            {
                handler.second();
            }
            catch (...)
            {
                LOG_ERROR("multiplayer_session_changed call threw an exception");
            }
        }
    }
}

std::error_code
multiplayer_service_impl::enable_multiplayer_subscriptions()
{
    std::lock_guard<std::mutex> lock(m_subscriptionEnabledLock.get());
    if (m_realTimeActivityService == nullptr || m_subscriptionEnabled) 
    {
        return xbox_live_error_code::logic_error;
    }

    m_subscriptionEnabled = true;

    return xbox_live_error_code::no_error;
}

bool
multiplayer_service_impl::subscriptions_enabled()
{
    std::lock_guard<std::mutex> lock(m_subscriptionEnabledLock.get());
    return m_subscriptionEnabled;
}

void
multiplayer_service_impl::disable_multiplayer_subscriptions()
{
    if (m_realTimeActivityService == nullptr) return;

    {
        std::lock_guard<std::mutex> lock(m_subscriptionEnabledLock.get());
        m_subscriptionEnabled = false;
    }

    std::shared_ptr<multiplayer_subscription> subscription;
    {
        std::lock_guard<std::mutex> lock(m_subscriptionLock.get());
        subscription = m_subscription;
    }

    if (subscription)
    {
        m_realTimeActivityService->_Remove_subscription(
            subscription
            );

        {
            std::lock_guard<std::mutex> lock(m_subscriptionLock.get());
            m_subscription = nullptr;
        }
    }
    else
    {
        multiplayer_subscription_lost();
    }
}

function_context
multiplayer_service_impl::add_multiplayer_session_changed_handler(
    _In_ std::function<void(const multiplayer_session_change_event_args&)> handler
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock.get());

    function_context context = -1;
    if (m_realTimeActivityService != nullptr && handler != nullptr)
    {
        context = ++m_sessionChangeEventHandlerCounter;
        m_sessionChangeEventHandler[m_sessionChangeEventHandlerCounter] = std::move(handler);
    }

    return context;
}

void
multiplayer_service_impl::remove_multiplayer_session_changed_handler(
    _In_ function_context context
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock.get());
    if (m_realTimeActivityService == nullptr) return;

    m_sessionChangeEventHandler.erase(context);
}

function_context
multiplayer_service_impl::add_multiplayer_subscription_lost_handler(
    _In_ std::function<void()> handler
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock.get());

    function_context context = -1;
    if (m_realTimeActivityService != nullptr && handler != nullptr)
    {
        context = ++m_multiplayerSubscriptionLostEventHandlerCounter;
        m_multiplayerSubscriptionLostEventHandler[m_multiplayerSubscriptionLostEventHandlerCounter] = std::move(handler);
    }

    return context;
}

void
multiplayer_service_impl::remove_multiplayer_subscription_lost_handler(
    _In_ function_context context
    )
{
    std::lock_guard<std::mutex> lock(m_subscriptionLock.get());
    if (m_realTimeActivityService == nullptr) return;
    
    m_multiplayerSubscriptionLostEventHandler.erase(context);
}
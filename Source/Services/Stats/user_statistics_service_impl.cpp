//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "pch.h"
#include "xsapi/user_statistics.h"
#include "user_statistics_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_BEGIN

user_statistics_service_impl::user_statistics_service_impl(
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> realTimeActivityService
    ) :
    m_realTimeActivityService(realTimeActivityService),
    m_statisticChangeHandlerCounter(0)
{
}

user_statistics_service_impl::~user_statistics_service_impl()
{
    m_statisticChangeHandler.clear();
}

function_context
user_statistics_service_impl::add_statistic_changed_handler(
    _In_ std::function<void(const statistic_change_event_args&)> handler
    )
{
    std::lock_guard<std::mutex> lock(m_statisticHandlerLock.get());

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_statisticChangeHandlerCounter;
        m_statisticChangeHandler[m_statisticChangeHandlerCounter] = std::move(handler);
    }

    return context;
}

void
user_statistics_service_impl::remove_statistic_changed_handler(
    _In_ function_context context
    )
{
    std::lock_guard<std::mutex> lock(m_statisticHandlerLock.get());
    m_statisticChangeHandler.erase(context);
}

void
user_statistics_service_impl::statistic_changed(
    _In_ const statistic_change_event_args& eventArgs
    )
{
    std::unordered_map<function_context, std::function<void(const statistic_change_event_args&)>> statisticChangeHandlerCopy;
    {
        std::lock_guard<std::mutex> lock(m_statisticHandlerLock.get());
        statisticChangeHandlerCopy = m_statisticChangeHandler;
    }

    for (auto& handler : statisticChangeHandlerCopy)
    {
        XSAPI_ASSERT(handler.second != nullptr);
        if (handler.second != nullptr)
        {
            try
            {
                handler.second(eventArgs);
            }
            catch (...)
            {
                LOG_ERROR("statistic_changed handler threw an exception");
            }
        }
    }
}

xbox_live_result<std::shared_ptr<statistic_change_subscription>>
user_statistics_service_impl::subscribe_to_statistic_change(
    _In_ const string_t& xboxUserId,
    _In_ const string_t& serviceConfigurationId,
    _In_ const string_t& statisticName
    )
{
    std::weak_ptr<user_statistics_service_impl> thisWeakPtr = shared_from_this();

    std::shared_ptr<statistic_change_subscription> statChangeSub = std::make_shared<statistic_change_subscription>(
        xboxUserId,
        serviceConfigurationId,
        statistic(
            statisticName,
            string_t(),
            string_t()
            ),
        ([thisWeakPtr](const statistic_change_event_args& eventArgs)
        {
            std::shared_ptr<user_statistics_service_impl> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->statistic_changed(eventArgs);
            }
        }),
        ([thisWeakPtr](const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args& eventArgs)
        {
            std::shared_ptr<user_statistics_service_impl> pThis(thisWeakPtr.lock());
            if (pThis != nullptr)
            {
                pThis->m_realTimeActivityService->_Trigger_subscription_error(eventArgs);
            }
        })
        );

    auto subscriptionSucceeded = m_realTimeActivityService->_Add_subscription(
        statChangeSub
        );

    if (!subscriptionSucceeded.err())
    {
        return xbox_live_result<std::shared_ptr<statistic_change_subscription>>(statChangeSub);
    }

    return xbox_live_result<std::shared_ptr<statistic_change_subscription>>(subscriptionSucceeded.err(), subscriptionSucceeded.err_message());
}

xbox_live_result<void>
user_statistics_service_impl::unsubscribe_from_statistic_change(
    _In_ std::shared_ptr<statistic_change_subscription> subscription
    )
{
    return m_realTimeActivityService->_Remove_subscription(subscription);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_USERSTATISTICS_CPP_END
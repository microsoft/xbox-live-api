// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/tournaments.h"
#include "tournament_service_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_BEGIN

tournament_service_impl::tournament_service_impl(
    _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> rtaService
    ) :
    m_realTimeActivityService(rtaService),
    m_tournamentChangeHandlerCounter(0),
    m_teamChangeHandlerCounter(0)
{
}

tournament_service_impl::~tournament_service_impl()
{
    m_tournamentChangeHandler.clear();
    m_teamChangeHandler.clear();
}

function_context
tournament_service_impl::add_tournament_changed_handler(
    _In_ std::function<void(const tournament_change_event_args&)> handler
)
{
    std::lock_guard<std::mutex> lock(m_tournamentHandlerLock.get());

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_tournamentChangeHandlerCounter;
        m_tournamentChangeHandler[m_tournamentChangeHandlerCounter] = std::move(handler);
    }

    return context;
}

void
tournament_service_impl::remove_tournament_changed_handler(
    _In_ function_context context
)
{
    std::lock_guard<std::mutex> lock(m_tournamentHandlerLock.get());
    m_tournamentChangeHandler.erase(context);
}

void
tournament_service_impl::tournament_changed(
    _In_ const tournament_change_event_args& eventArgs
)
{
    std::unordered_map<function_context, std::function<void(const tournament_change_event_args&)>> tournamentChangeHandlerCopy;
    {
        std::lock_guard<std::mutex> lock(m_tournamentHandlerLock.get());
        tournamentChangeHandlerCopy = m_tournamentChangeHandler;
    }

    for (auto& handler : tournamentChangeHandlerCopy)
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
                LOG_ERROR("tournament_changed handler threw an exception");
            }
        }
    }
}

xbox_live_result<std::shared_ptr<tournament_change_subscription>>
tournament_service_impl::subscribe_to_tournament_change(
    _In_ const string_t& organizerId,
    _In_ const string_t& tournamentId
    )
{
    std::weak_ptr<tournament_service_impl> thisWeakPtr = shared_from_this();

    std::shared_ptr<tournament_change_subscription> statChangeSub = std::make_shared<tournament_change_subscription>(
        organizerId,
        tournamentId,
    ([thisWeakPtr](const tournament_change_event_args& eventArgs)
    {
        std::shared_ptr<tournament_service_impl> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->tournament_changed(eventArgs);
        }
    }),
            ([thisWeakPtr](const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args& eventArgs)
    {
        std::shared_ptr<tournament_service_impl> pThis(thisWeakPtr.lock());
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
        return xbox_live_result<std::shared_ptr<tournament_change_subscription>>(statChangeSub);
    }

    return xbox_live_result<std::shared_ptr<tournament_change_subscription>>(subscriptionSucceeded.err(), subscriptionSucceeded.err_message());
}

xbox_live_result<void>
tournament_service_impl::unsubscribe_from_tournament_change(
    _In_ std::shared_ptr<tournament_change_subscription> subscription
)
{
    return m_realTimeActivityService->_Remove_subscription(subscription);
}


function_context
tournament_service_impl::add_team_changed_handler(
    _In_ std::function<void(const team_change_event_args&)> handler
)
{
    std::lock_guard<std::mutex> lock(m_teamHandlerLock.get());

    function_context context = -1;
    if (handler != nullptr)
    {
        context = ++m_teamChangeHandlerCounter;
        m_teamChangeHandler[m_teamChangeHandlerCounter] = std::move(handler);
    }

    return context;
}

void
tournament_service_impl::remove_team_changed_handler(
    _In_ function_context context
)
{
    std::lock_guard<std::mutex> lock(m_teamHandlerLock.get());
    m_teamChangeHandler.erase(context);
}

void
tournament_service_impl::team_changed(
    _In_ const team_change_event_args& eventArgs
    )
{
    std::unordered_map<function_context, std::function<void(const team_change_event_args&)>> teamChangeHandlerCopy;
    {
        std::lock_guard<std::mutex> lock(m_teamHandlerLock.get());
        teamChangeHandlerCopy = m_teamChangeHandler;
    }

    for (auto& handler : teamChangeHandlerCopy)
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
                LOG_ERROR("team_changed handler threw an exception");
            }
        }
    }
}

xbox_live_result<std::shared_ptr<team_change_subscription>>
tournament_service_impl::subscribe_to_team_change(
    _In_ const string_t& organizerId,
    _In_ const string_t& tournamentId,
    _In_ const string_t& teamId
    )
{
    std::weak_ptr<tournament_service_impl> thisWeakPtr = shared_from_this();

    std::shared_ptr<team_change_subscription> statChangeSub = std::make_shared<team_change_subscription>(
        organizerId,
        tournamentId,
        teamId,
    ([thisWeakPtr](const team_change_event_args& eventArgs)
    {
        std::shared_ptr<tournament_service_impl> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            pThis->team_changed(eventArgs);
        }
    }),
    ([thisWeakPtr](const xbox::services::real_time_activity::real_time_activity_subscription_error_event_args& eventArgs)
    {
        std::shared_ptr<tournament_service_impl> pThis(thisWeakPtr.lock());
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
        return xbox_live_result<std::shared_ptr<team_change_subscription>>(statChangeSub);
    }

    return xbox_live_result<std::shared_ptr<team_change_subscription>>(subscriptionSucceeded.err(), subscriptionSucceeded.err_message());
}

xbox_live_result<void>
tournament_service_impl::unsubscribe_from_team_change(
    _In_ std::shared_ptr<team_change_subscription> subscription
    )
{
    return m_realTimeActivityService->_Remove_subscription(subscription);
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_TOURNAMENTS_CPP_END
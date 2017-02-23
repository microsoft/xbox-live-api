// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "system_internal.h"
namespace xbox { namespace services { namespace user_statistics {

class user_statistics_service_impl : public std::enable_shared_from_this<user_statistics_service_impl>
{
public:
    user_statistics_service_impl(
        _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> realTimeActivityService
        );

    ~user_statistics_service_impl();

    xbox_live_result<std::shared_ptr<statistic_change_subscription>> subscribe_to_statistic_change(
        _In_ const string_t& xboxUserId,
        _In_ const string_t& serviceConfigurationId,
        _In_ const string_t& statisticName
        );

    xbox_live_result<void> unsubscribe_from_statistic_change(
        _In_ std::shared_ptr<statistic_change_subscription> subscription
        );

    function_context add_statistic_changed_handler(_In_ std::function<void(const statistic_change_event_args&)> handler);

    void remove_statistic_changed_handler(_In_ function_context context);

private:
    void statistic_changed(_In_ const statistic_change_event_args& eventArgs);

    xbox::services::system::xbox_live_mutex m_statisticHandlerLock;
    std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> m_realTimeActivityService;
    std::unordered_map<function_context, std::function<void(const statistic_change_event_args&)>> m_statisticChangeHandler;
    function_context m_statisticChangeHandlerCounter;
};

}}}
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "system_internal.h"
namespace xbox { namespace services { namespace tournaments {

class tournament_service_impl : public std::enable_shared_from_this<tournament_service_impl>
{
public:
    tournament_service_impl(
        _In_ std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> rtaService
        );

    ~tournament_service_impl();

    xbox_live_result<std::shared_ptr<team_change_subscription>> subscribe_to_team_change(
        _In_ const string_t& organizerId,
        _In_ const string_t& tournamentId,
        _In_ const string_t& teamId
        );

    xbox_live_result<void> unsubscribe_from_team_change(
        _In_ std::shared_ptr<team_change_subscription> subscription
        );

    function_context add_team_changed_handler(_In_ std::function<void(const team_change_event_args&)> handler);

    void remove_team_changed_handler(_In_ function_context context);

private:
    void team_changed(_In_ const team_change_event_args& eventArgs);

    xbox::services::system::xbox_live_mutex m_teamHandlerLock;
    std::shared_ptr<xbox::services::real_time_activity::real_time_activity_service> m_realTimeActivityService;
    std::unordered_map<function_context, std::function<void(const team_change_event_args&)>> m_teamChangeHandler;
    function_context m_teamChangeHandlerCounter;
};

}}}
// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#pragma once
#include "xsapi/social_manager.h"
#include "social_manager_internal.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

class MockSocialGraph : public social_graph
{
public:
    MockSocialGraph(
        _In_ xbox_live_user_t user,
        _In_ social_manager_extra_detail_level socialManagerExtraDetailLevel,
        _In_ std::function<void()> graphDestructionCompleteCallback,
        _In_ async_queue_handle_t queue
        );
    std::shared_ptr<xbox_live_context_impl> internal_xbox_live_context_impl() const;
    void add_state_handler(_In_ const std::function<void(_In_ xbox::services::real_time_activity::real_time_activity_connection_state state)>& stateRTAFunction);
    const unprocessed_event_queue& internal_queue();
};

class MockSocialManager : public social_manager
{
public:
    static std::shared_ptr<social_manager> get_mock_singleton_instance();

    xsapi_internal_unordered_map<string_t, std::shared_ptr<xbox_social_user_group>> xbox_social_user_groups() const;
    xsapi_internal_unordered_map<string_t, xsapi_internal_vector<string_t>> user_to_view_map() const;
    xsapi_internal_unordered_map<string_t, std::shared_ptr<MockSocialGraph>> local_graphs();
    std::vector<social_event> event_queue() const;
    std::vector<xbox_live_user_t> local_user_list() const;
    bool social_graph_event_queue_empty(_In_ const string_t& xuid);
};

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END
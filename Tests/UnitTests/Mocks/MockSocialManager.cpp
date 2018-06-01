// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "MockSocialManager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

static std::shared_ptr<MockSocialManager> g_socialManager;

std::shared_ptr<social_manager>
MockSocialManager::get_mock_singleton_instance()
{
    static std::mutex socialManagerInitLock;
    std::lock_guard<std::mutex> lock(socialManagerInitLock);
    if (g_socialManager == nullptr)
    {
        g_socialManager = std::shared_ptr<MockSocialManager>(new MockSocialManager());
    }
    return g_socialManager;
}

xsapi_internal_unordered_map<string_t, std::shared_ptr<xbox_social_user_group>> MockSocialManager::xbox_social_user_groups() const
{
    xsapi_internal_unordered_map<string_t, std::shared_ptr<xbox_social_user_group>> groups;
    for (auto& group : m_internalObj->m_xboxSocialUserGroups)
    {
        groups[utils::string_t_from_internal_string(group.first)] = std::make_shared<xbox_social_user_group>(group.second);
    }
    return groups;
}

xsapi_internal_unordered_map<string_t, xsapi_internal_vector<string_t>> MockSocialManager::user_to_view_map() const
{
    xsapi_internal_unordered_map<string_t, xsapi_internal_vector<string_t>> usersToView;
    for (auto& userToView : m_internalObj->m_userToViewMap)
    {
        xsapi_internal_vector<string_t> string_t_vector;
        for (auto& internalString : userToView.second)
        {
            string_t_vector.push_back(utils::string_t_from_internal_string(internalString));
        }
        usersToView[utils::string_t_from_internal_string(userToView.first)] = string_t_vector;
    }
    return usersToView;
}

xsapi_internal_unordered_map<string_t, std::shared_ptr<MockSocialGraph>> MockSocialManager::local_graphs()
{
    xsapi_internal_unordered_map<string_t, std::shared_ptr<MockSocialGraph>> mockGraphs;
    for (auto& graph : m_internalObj->m_localGraphs)
    {
        mockGraphs[utils::string_t_from_internal_string(graph.first)] = std::dynamic_pointer_cast<MockSocialGraph>(graph.second);
    }

    return mockGraphs;
}

std::vector<social_event> MockSocialManager::event_queue() const
{
    return utils::std_vector_external_from_internal_vector<social_event, std::shared_ptr<social_event_internal>>(
        m_internalObj->m_eventQueue
        );
}

std::vector<xbox_live_user_t> MockSocialManager::local_user_list() const
{
    return utils::std_vector_from_internal_vector(m_internalObj->m_localUserList);
}

bool
MockSocialManager::social_graph_event_queue_empty(
    _In_ const string_t& xuid
    )
{
    return m_internalObj->m_localGraphs[utils::internal_string_from_string_t(xuid)]->are_events_empty();
}
MockSocialGraph::MockSocialGraph(
    _In_ xbox_live_user_t user,
    _In_ social_manager_extra_detail_level socialManagerExtraDetailLevel,
    _In_ std::function<void()> graphDestructionCompleteCallback,
    _In_ async_queue_handle_t queue
    ) : social_graph(user, socialManagerExtraDetailLevel, graphDestructionCompleteCallback, queue)
{
}

std::shared_ptr<xbox_live_context_impl>
MockSocialGraph::internal_xbox_live_context_impl() const
{
    return m_xboxLiveContextImpl;
}

void
MockSocialGraph::add_state_handler(
    _In_ const std::function<void(_In_ xbox::services::real_time_activity::real_time_activity_connection_state state)>& stateRTAFunction
)
{
    m_stateRTAFunction = stateRTAFunction;
}

const unprocessed_event_queue&
MockSocialGraph::internal_queue()
{
    return m_unprocessedEventQueue;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END
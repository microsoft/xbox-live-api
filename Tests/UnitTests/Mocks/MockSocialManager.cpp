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
#include "MockSocialManager.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

std::shared_ptr<social_manager>
MockSocialManager::get_mock_singleton_instance()
{
    static std::mutex socialManagerInitLock;
    std::lock_guard<std::mutex> lock(socialManagerInitLock);
    if (m_socialManager == nullptr)
    {
        m_socialManager = std::shared_ptr<MockSocialManager>(new MockSocialManager());
    }
    return m_socialManager;
}

const xsapi_internal_unordered_map(string_t, std::shared_ptr<xbox_social_user_group>)& MockSocialManager::xbox_social_user_groups() const
{
    return m_xboxSocialUserGroups;
}
const xsapi_internal_unordered_map(string_t, xsapi_internal_vector(string_t))& MockSocialManager::user_to_view_map() const
{
    return m_userToViewMap;
}

const xsapi_internal_unordered_map(string_t, std::shared_ptr<MockSocialGraph>) MockSocialManager::local_graphs()
{
    xsapi_internal_unordered_map(string_t, std::shared_ptr<MockSocialGraph>) mockGraphs;
    for (auto& graph : m_localGraphs)
    {
        mockGraphs[graph.first] = std::dynamic_pointer_cast<MockSocialGraph>(graph.second);
    }

    return mockGraphs;
}

const std::vector<social_event>& MockSocialManager::event_queue() const
{
    return m_eventQueue;
}

const std::vector<xbox_live_user_t>& MockSocialManager::local_user_list() const
{
    return m_localUserList;
}

bool
MockSocialManager::social_graph_event_queue_empty(
    _In_ const string_t& xuid
    )
{
    return m_localGraphs[xuid]->are_events_empty();
}
MockSocialGraph::MockSocialGraph(
    _In_ xbox_live_user_t user,
    _In_ social_manager_extra_detail_level socialManagerExtraDetailLevel,
    _In_ std::function<void()> graphDestructionCompleteCallback
    ) : social_graph(user, socialManagerExtraDetailLevel, graphDestructionCompleteCallback)
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

const internal_event_queue&
MockSocialGraph::internal_queue()
{
    return m_internalEventQueue;
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END
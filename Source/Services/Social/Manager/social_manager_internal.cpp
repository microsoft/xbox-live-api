// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "social_manager_internal.h"
#if UNIT_TEST_SERVICES
#include "MockSocialManager.h"
#endif

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

using namespace xbox::services::system;

std::shared_ptr<social_manager_internal>
social_manager_internal::get_singleton_instance()
{
    auto xsapiSingleton = get_xsapi_singleton();
    std::lock_guard<std::recursive_mutex> lock(xsapiSingleton->m_socialManagerLock);
    if (xsapiSingleton->m_socialManagerInternalInstance == nullptr)
    {
        auto buffer = xsapi_memory::mem_alloc(sizeof(social_manager_internal));
        xsapiSingleton->m_socialManagerInternalInstance = std::shared_ptr<social_manager_internal>(
            new (buffer) social_manager_internal(),
            xsapi_alloc_deleter<social_manager_internal>()
            );

        xsapiSingleton->m_perfTester = xsapi_allocate_shared<xbox::services::perf_tester>("social_manager_internal");
    }
    return xsapiSingleton->m_socialManagerInternalInstance;
}

social_manager_internal::social_manager_internal()
{
    m_backgroundAsyncQueue = get_xsapi_singleton()->m_asyncQueue;
}

xbox_live_result<std::shared_ptr<xbox_social_user_group_internal>>
social_manager_internal::create_social_user_group_from_filters(
    _In_ xbox_live_user_t user,
    _In_ presence_filter presenceFilterLevel,
    _In_ relationship_filter relationshipFilter
)
{
    std::lock_guard<std::recursive_mutex> lock(m_socialMangerLock);
    if (user == nullptr)
    {
        return xbox_live_result<std::shared_ptr<xbox_social_user_group_internal>>(xbox_live_error_code::invalid_argument, "user is null");
    }

    xsapi_internal_string ownerUserId = user_context::get_user_id(user);

    if (m_localGraphs.find(ownerUserId) == m_localGraphs.end())
    {
        return xbox_live_result<std::shared_ptr<xbox_social_user_group_internal>>(
            xbox_live_error_code::logic_error,
            "Add local user to graph before creating a social user group for them"
            );
    }

    xsapi_internal_string viewHash = utils::create_guid(true);

    uint32_t titleId = m_localGraphs[ownerUserId]->title_id();
    auto socialGroup = xsapi_allocate_shared<xbox_social_user_group_internal>(
        viewHash,
        presenceFilterLevel,
        relationshipFilter,
        titleId,
        user
        );

    m_userToViewMap[ownerUserId].push_back(viewHash);
    m_xboxSocialUserGroups[viewHash] = socialGroup;

    if (m_localGraphs[ownerUserId]->is_initialized())
    {

        m_xboxSocialUserGroups[viewHash]->initialize_filter_list(
            *m_localGraphs[ownerUserId]->active_buffer_social_graph()
        );

        std::lock_guard<std::recursive_mutex> eventLock(m_socialManagerEventLock);
        m_eventQueue.push_back(
            xsapi_allocate_shared<social_event_internal>(
                user,
                social_event_type::social_user_group_loaded,
                xsapi_internal_vector<xbox_user_id_container>(),
                std::make_shared<social_user_group_loaded_event_args_internal>(socialGroup)
            )
        );
    }
    return socialGroup;
}

xbox_live_result<std::shared_ptr<xbox_social_user_group_internal>>
social_manager_internal::create_social_user_group_from_list(
    _In_ xbox_live_user_t user,
    _In_ xsapi_internal_vector<xsapi_internal_string> xboxUserIdList
)
{
    if (xboxUserIdList.empty())
    {
        return xbox_live_result<std::shared_ptr<xbox_social_user_group_internal>>(xbox_live_error_code::invalid_argument, "xboxUserIdList cannot be empty");
    }
    else if (xboxUserIdList.size() > MAX_USERS_FROM_LIST)
    {
        return xbox_live_result<std::shared_ptr<xbox_social_user_group_internal>>(xbox_live_error_code::invalid_argument, "xboxUserIdList has a limit of 100 users");
    }

    std::lock_guard<std::recursive_mutex> lock(m_socialMangerLock);
    auto ownerUserId = user_context::get_user_id(user);

    auto userGraph = m_localGraphs.find(ownerUserId);
    if (userGraph == m_localGraphs.end())
    {
        return xbox_live_result<std::shared_ptr<xbox_social_user_group_internal>>(
            xbox_live_error_code::logic_error,
            "Add local user to graph before creating a social user group for them"
            );
    }

    xsapi_internal_string viewHash = utils::create_guid(true);
    auto socialGroup = std::make_shared<xbox_social_user_group_internal>(
        viewHash,
        xboxUserIdList,
        user
        );

    auto hash = socialGroup->hash();

    std::weak_ptr<social_manager_internal> thisWeakPtr = shared_from_this();

    userGraph->second->add_users(xboxUserIdList,
    [thisWeakPtr, user, socialGroup, hash, ownerUserId](xbox_live_result<void> users)
    {
        std::shared_ptr<social_manager_internal> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            std::lock_guard<std::recursive_mutex> eventLock(pThis->m_socialManagerEventLock);

            if (users.err())
            {
                pThis->m_eventQueue.push_back(
                    xsapi_allocate_shared<social_event_internal>(
                        user,
                        social_event_type::social_user_group_loaded,
                        xsapi_internal_vector<xbox_user_id_container>(),
                        std::make_shared<social_user_group_loaded_event_args_internal>(socialGroup),
                        users.err(),
                        users.err_message().data()
                    )
                );
            }
            else
            {
                xsapi_internal_vector<xbox_user_id_container> usersAffected;
                for (auto& trackedUser : socialGroup->users_tracked_by_social_user_group())
                {
                    usersAffected.push_back(trackedUser);
                }

                pThis->m_eventQueue.push_back(
                    xsapi_allocate_shared<social_event_internal>(
                        user,
                        social_event_type::social_user_group_loaded,
                        usersAffected,
                        std::make_shared<social_user_group_loaded_event_args_internal>(socialGroup)
                    )
                );

            }
        }
    });

    m_userToViewMap[ownerUserId].push_back(viewHash);
    m_xboxSocialUserGroups[viewHash] = socialGroup;
    return socialGroup;
}

xbox_live_result<void>
social_manager_internal::destroy_social_user_group(
    _In_ std::shared_ptr<xbox_social_user_group_internal> socialGroup
)
{
    std::lock_guard<std::recursive_mutex> lock(m_socialMangerLock);

    if (m_xboxSocialUserGroups.find(socialGroup->hash()) == m_xboxSocialUserGroups.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "Social user group not found");
    }

    // Remove the entry from user to view map
    xsapi_internal_string user;
    bool isFound = false;
    for (auto& map : m_userToViewMap)
    {
        for (auto itr = map.second.begin(); itr < map.second.end(); ++itr)
        {
            auto viewGuid = *itr;
            if (socialGroup->hash() == viewGuid)
            {
                user = map.first;
                map.second.erase(itr);
                isFound = true;
                break;
            }
        }
        if (isFound)
        {
            break;
        }
    }

    auto& viewToDestroy = m_xboxSocialUserGroups[socialGroup->hash()];
    xsapi_internal_vector<uint64_t> trackingList = viewToDestroy->tracking_users();
    if (socialGroup->social_user_group_type() != social_user_group_type::filter_type)
    {
        m_localGraphs[user]->remove_users(trackingList);
    }
    viewToDestroy->destroy();

    m_xboxSocialUserGroups.erase(socialGroup->hash());


    return xbox_live_result<void>();
}

xbox_live_result<void>
social_manager_internal::add_local_user(
    _In_ xbox_live_user_t user,
    _In_ social_manager_extra_detail_level extraDetailLevel
)
{
    if (user == nullptr)
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "user object is null");
    }

    auto userString = user_context::get_user_id(user);

    {
        std::lock_guard<std::recursive_mutex> lock(m_socialMangerLock);

        if (m_localGraphs.find(userString) != m_localGraphs.end())
        {
            return xbox_live_result<void>(xbox_live_error_code::logic_error, "user already exists in graph");
        }

        std::weak_ptr<social_manager_internal> thisWeakPtr = shared_from_this();

#if UNIT_TEST_SERVICES
        auto newGraph = std::make_shared<MockSocialGraph>(
#else
        auto newGraph = xsapi_allocate_shared<social_graph>(
#endif
                user,
                extraDetailLevel,
                [thisWeakPtr, user]()
        {
            std::shared_ptr<social_manager_internal> pThis(thisWeakPtr.lock());
            if (pThis)
            {
                std::lock_guard<std::recursive_mutex> eventLock(pThis->m_socialManagerEventLock);
                auto userRemovedEvent = xsapi_allocate_shared<social_event_internal>(
                    user,
                    social_event_type::local_user_removed,
                    xsapi_internal_vector<xbox_user_id_container>()
                );
                pThis->m_eventQueue.push_back(userRemovedEvent);
            }
        }
        , m_backgroundAsyncQueue
        );

        m_localGraphs[userString] = newGraph;

        newGraph->initialize([thisWeakPtr, user, userString](xbox_live_result<void> result)
        {
            std::shared_ptr<social_manager_internal> pThis(thisWeakPtr.lock());
            if (pThis)
            {
                if (result.err())
                {
                    std::lock_guard<std::recursive_mutex> eventLock(pThis->m_socialManagerEventLock);
                    pThis->m_eventQueue.push_back(
                        xsapi_allocate_shared<social_event_internal>(
                            user,
                            social_event_type::local_user_added,
                            xsapi_internal_vector<xbox_user_id_container>(),
                            std::make_shared<social_event_args>(),
                            result.err(),
                            result.err_message().data()
                        )
                    );
                }
                else
                {
                    std::lock_guard<std::recursive_mutex> lock(pThis->m_socialMangerLock);

                    auto viewList = pThis->m_userToViewMap[userString];
                    if (!viewList.empty())
                    {
                        for (auto& view : viewList)
                        {
                            if (pThis->m_xboxSocialUserGroups.find(view) != pThis->m_xboxSocialUserGroups.end())
                            {
                                auto currentView = pThis->m_xboxSocialUserGroups[view];
                                if (currentView->social_user_group_type() == social_user_group_type::filter_type)
                                {
                                    std::weak_ptr<social_manager_internal> socialManagerWeakPtr = pThis;
                                    currentView->initialize_filter_list(
                                        *pThis->m_localGraphs[userString]->active_buffer_social_graph()
                                    );

                                    std::lock_guard<std::recursive_mutex> eventLock(pThis->m_socialManagerEventLock);
                                    pThis->m_eventQueue.push_back(
                                        xsapi_allocate_shared<social_event_internal>(
                                            user,
                                            social_event_type::social_user_group_loaded,
                                            xsapi_internal_vector<xbox_user_id_container>(),
                                            std::make_shared<social_user_group_loaded_event_args_internal>(currentView)
                                        )
                                    );
                                }
                            }

                        }
                    }

                    std::lock_guard<std::recursive_mutex> eventLock(pThis->m_socialManagerEventLock);
                    pThis->m_eventQueue.push_back(
                        xsapi_allocate_shared<social_event_internal>(
                            user,
                            social_event_type::local_user_added,
                            xsapi_internal_vector<xbox_user_id_container>()
                        )
                    );
                }
            }
        });
    }

    {
        std::lock_guard<std::recursive_mutex> lock(m_socialMangerLock);
        m_localUserList.push_back(user);
    }
    return xbox_live_result<void>();
}

xbox_live_result<void>
social_manager_internal::remove_local_user(
    _In_ xbox_live_user_t user
)
{
    if (user == nullptr)
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User cannot be null");
    }

    std::lock_guard<std::recursive_mutex> lock(m_socialMangerLock);

    auto xboxUserId = user_context::get_user_id(user);
    auto graphIter = m_localGraphs.find(xboxUserId);
    if (graphIter == m_localGraphs.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::logic_error, "User not found in graph");
    }

    m_localGraphs.erase(xboxUserId);

    auto& currentView = m_userToViewMap[xboxUserId];
    for (auto& view : currentView)
    {
        if (m_xboxSocialUserGroups[view])
        {
            m_xboxSocialUserGroups[view]->destroy();
            m_xboxSocialUserGroups.erase(view);
        }
    }

    m_userToViewMap.erase(xboxUserId);

    xsapi_internal_vector<xbox_live_user_t> userList;
    uint32_t i;
    for (i = 0; i < m_localUserList.size(); ++i)
    {
        auto localUser = m_localUserList[i];
        auto userId = user_context::get_user_id(user);
        auto localUserId = user_context::get_user_id(localUser);
        if (utils::str_icmp(localUserId, userId) != 0)
        {
            userList.push_back(localUser);
        }
    }

    m_localUserList = userList;
    return xbox_live_result<void>();
}

xsapi_internal_vector<std::shared_ptr<social_event_internal>>
social_manager_internal::do_work()
{
    std::lock_guard<std::recursive_mutex> lock(m_socialMangerLock);
    std::lock_guard<std::recursive_mutex> eventLock(m_socialManagerEventLock);
    xsapi_internal_vector<std::shared_ptr<social_event_internal>> socialEvents(m_eventQueue);
    auto xsapiSingleton = get_xsapi_singleton();
    xsapiSingleton->m_perfTester->start_timer("do_work");
    xsapiSingleton->m_perfTester->start_timer("do_work: eventqueue clear");
    m_eventQueue.clear();
    xsapiSingleton->m_perfTester->stop_timer("do_work: eventqueue clear");
    for (auto& graph : m_localGraphs)
    {
        xsapiSingleton->m_perfTester->start_timer("do_work: social_graph do_work");
        auto graphData = graph.second->do_work(socialEvents);
        xsapiSingleton->m_perfTester->stop_timer("do_work: social_graph do_work");
        const auto& userViewList = m_userToViewMap[graph.first];
        for (auto& viewHash : userViewList)
        {
            auto& view = m_xboxSocialUserGroups[viewHash];
            if (graphData.socialUsers != nullptr)
            {
                xsapiSingleton->m_perfTester->start_timer("do_work: update_view");
                view->update_view(*graphData.socialUsers, socialEvents);
                xsapiSingleton->m_perfTester->stop_timer("do_work: update_view");
            }
        }
    }

    xsapiSingleton->m_perfTester->stop_timer("do_work");
    xsapiSingleton->m_perfTester->clear();
    return socialEvents;
}

const xsapi_internal_vector<xbox_live_user_t>&
social_manager_internal::local_users() const
{
    return m_localUserList;
}

xbox_live_result<void> social_manager_internal::update_social_user_group(
    _In_ const std::shared_ptr<xbox_social_user_group_internal>& socialGroup,
    _In_ const xsapi_internal_vector<xsapi_internal_string>& users
)
{
    std::lock_guard<std::recursive_mutex> lock(m_socialMangerLock);

    if (m_xboxSocialUserGroups.find(socialGroup->hash()) == m_xboxSocialUserGroups.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "Social user group not found");
    }

    if (socialGroup->social_user_group_type() != social_user_group_type::user_list_type)
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "Users can only be added to user groups of user_list_type");
    }

    if (users.size() > MAX_USERS_FROM_LIST)
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "users cannot be larger than 100 users");
    }

    auto diffUsers = socialGroup->update_users_in_group(users);

    auto localUser = socialGroup->local_user();
    xsapi_internal_string xuid = user_context::get_user_id(localUser);
    m_localGraphs[xuid]->remove_users(diffUsers.removeGroup);
    std::weak_ptr<social_manager_internal> thisWeakPtr = shared_from_this();

    auto hash = socialGroup->hash();
    m_localGraphs[xuid]->add_users(diffUsers.addGroup,
    [thisWeakPtr, localUser, users, hash](xbox_live_result<void> userResult)
    {
        std::shared_ptr<social_manager_internal> pThis(thisWeakPtr.lock());
        if (pThis)
        {
            std::lock_guard<std::recursive_mutex> eventLock(pThis->m_socialManagerEventLock);

            // send event
            pThis->m_eventQueue.push_back(
                xsapi_allocate_shared<social_event_internal>(
                    localUser,
                    social_event_type::social_user_group_updated,
                    xsapi_internal_vector<xbox_user_id_container>(),
                    nullptr,
                    userResult.err(),
                    userResult.err_message().data()
                )
            );
        }
    });

    return xbox_live_result<void>();
}

xbox_live_result<void>
social_manager_internal::set_rich_presence_polling_status(
    _In_ xbox_live_user_t user,
    _In_ bool shouldEnablePolling
)
{
    std::lock_guard<std::recursive_mutex> lock(m_socialMangerLock);
    auto localGraphIter = m_localGraphs.find(user_context::get_user_id(user));
    if (localGraphIter == m_localGraphs.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::logic_error, "User not found in graph");
    }

    localGraphIter->second->enable_rich_presence_polling(shouldEnablePolling);
    return xbox_live_result<void>();
}

void social_manager_internal::set_social_graph_background_async_queue(async_queue_handle_t queue)
{
    std::lock_guard<std::recursive_mutex> lock(m_socialMangerLock);
    m_backgroundAsyncQueue = queue;
    for (auto& graph : m_localGraphs)
    {
        graph.second->set_background_async_queue(queue);
    }
}

xbox_services_diagnostics_trace_level
social_manager_internal::diagnostics_trace_level() const
{
    return m_traceLevel;
}

void
social_manager_internal::set_diagnostics_trace_level(
    _In_ xbox_services_diagnostics_trace_level traceLevel
)
{
    m_traceLevel = traceLevel;
}

void social_manager_internal::log_state()
{
    LOGS_DEBUG_IF(social_manager_internal::get_singleton_instance()->diagnostics_trace_level() >= xbox_services_diagnostics_trace_level::verbose) 
        << "[SM] State: m_xboxSocialUserGroups: " << m_xboxSocialUserGroups.size()
        << " m_userToViewMap: " << m_userToViewMap.size()
        << " m_localGraphs: " << m_localGraphs.size()
        << " m_eventQueue: " << m_eventQueue.size()
        << " m_localUserList: " << m_localUserList.size();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END

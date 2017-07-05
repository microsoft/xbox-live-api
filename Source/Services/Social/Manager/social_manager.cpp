// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "pch.h"
#include "xsapi/social_manager.h"
#include "social_manager_internal.h"
#if UNIT_TEST_SERVICES
#include "MockSocialManager.h"
#endif

#include "perf_tester.h"

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_BEGIN

std::shared_ptr<social_manager>
social_manager::get_singleton_instance()
{
    auto xsapiSingleton = get_xsapi_singleton();
    std::lock_guard<std::mutex> lock(xsapiSingleton->m_singletonLock);
    if (xsapiSingleton->m_socialManagerInstance == nullptr)
    {
        xsapiSingleton->m_socialManagerInstance = std::shared_ptr<social_manager>(new social_manager());
        xsapiSingleton->m_perfTester = std::make_shared<xbox::services::perf_tester>(_T("social_manager"));
    }
    return xsapiSingleton->m_socialManagerInstance;
}

social_manager::social_manager()
{
}

xbox_live_result<std::shared_ptr<xbox_social_user_group>>
social_manager::create_social_user_group_from_filters(
    _In_ xbox_live_user_t user,
    _In_ presence_filter presenceFilterLevel,
    _In_ relationship_filter relationshipFilter
    )
{
    std::lock_guard<std::mutex> lock(m_socialMangerLock);
    if (user == nullptr) return xbox_live_result<std::shared_ptr<xbox_social_user_group>>(xbox_live_error_code::invalid_argument, "user is null");

    string_t ownerUserId = user_context::get_user_id(user);

    if (m_localGraphs.find(ownerUserId) == m_localGraphs.end())
    {
        return xbox_live_result<std::shared_ptr<xbox_social_user_group>>(
            xbox_live_error_code::logic_error,
            "Add local user to graph before creating a social user group for them"
            );
    }

    auto viewHash = utils::create_guid(true);

    uint32_t titleId = m_localGraphs[ownerUserId]->title_id();
    auto socialGroup = std::make_shared<xbox_social_user_group>(
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

        std::lock_guard<std::mutex> eventLock(m_socialManagerEventLock);
        m_eventQueue.push_back(
            social_event(
                user,
                social_event_type::social_user_group_loaded,
                std::vector<xbox_user_id_container>(),
                std::make_shared<social_user_group_loaded_event_args>(socialGroup)
                )
            );
    }
    return socialGroup;
}

xbox_live_result<std::shared_ptr<xbox_social_user_group>>
social_manager::create_social_user_group_from_list(
    _In_ xbox_live_user_t user,
    _In_ std::vector<string_t> xboxUserIdList
    )
{
    if (xboxUserIdList.empty())
    {
        return xbox_live_result<std::shared_ptr<xbox_social_user_group>>(xbox_live_error_code::invalid_argument, "xboxUserIdList cannot be empty");
    }
    else if (xboxUserIdList.size() > MAX_USERS_FROM_LIST)
    {
        return xbox_live_result<std::shared_ptr<xbox_social_user_group>>(xbox_live_error_code::invalid_argument, "xboxUserIdList has a limit of 100 users");
    }

    std::lock_guard<std::mutex> lock(m_socialMangerLock);
    string_t ownerUserId = user_context::get_user_id(user);

    auto userGraph = m_localGraphs.find(ownerUserId);
    if (userGraph == m_localGraphs.end())
    {
        return xbox_live_result<std::shared_ptr<xbox_social_user_group>>(
            xbox_live_error_code::logic_error,
            "Add local user to graph before creating a social user group for them"
            );
    }

    auto viewHash = utils::create_guid(true);
    auto socialGroup = std::make_shared<xbox_social_user_group>(
        viewHash,
        xboxUserIdList,
        user
        );

    auto hash = socialGroup->hash();

    std::weak_ptr<social_manager> thisWeakPtr = shared_from_this();

    pplx::task_completion_event<xbox_live_result<void>> tce;
    userGraph->second->add_users(xboxUserIdList, tce);

    create_task(tce).then([thisWeakPtr, user, socialGroup, hash, ownerUserId](xbox_live_result<void> users)
    {
        std::shared_ptr<social_manager> pThis(thisWeakPtr.lock());
        if (pThis != nullptr)
        {
            std::lock_guard<std::mutex> eventLock(pThis->m_socialManagerEventLock);

            if (users.err())
            {
                pThis->m_eventQueue.push_back(
                    social_event(
                        user,
                        social_event_type::social_user_group_loaded,
                        std::vector<xbox_user_id_container>(),
                        std::make_shared<social_user_group_loaded_event_args>(socialGroup),
                        users.err(),
                        users.err_message()
                        )
                    );
            }
            else
            {
                std::vector<xbox_user_id_container> usersAffected;
                for (auto& trackedUser : socialGroup->users_tracked_by_social_user_group())
                {
                    usersAffected.push_back(trackedUser);
                }
                
                pThis->m_eventQueue.push_back(
                    social_event(
                        user,
                        social_event_type::social_user_group_loaded,
                        usersAffected,
                        std::make_shared<social_user_group_loaded_event_args>(socialGroup)
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
social_manager::destroy_social_user_group(
    _In_ std::shared_ptr<xbox_social_user_group> socialGroup
    )
{
    std::lock_guard<std::mutex> lock(m_socialMangerLock);

    if (m_xboxSocialUserGroups.find(socialGroup->hash()) == m_xboxSocialUserGroups.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "Social user group not found");
    }

    // Remove the entry from user to view map
    string_t user;
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
    std::vector<uint64_t> trackingList = viewToDestroy->tracking_users();
    if (socialGroup->social_user_group_type() != social_user_group_type::filter_type)
    {
        m_localGraphs[user]->remove_users(trackingList);
    }
    viewToDestroy->destroy();

    m_xboxSocialUserGroups.erase(socialGroup->hash());


    return xbox_live_result<void>();
}

xbox_live_result<void>
social_manager::add_local_user(
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
        std::lock_guard<std::mutex> lock(m_socialMangerLock);

        if (m_localGraphs.find(userString) != m_localGraphs.end())
        {
            return xbox_live_result<void>(xbox_live_error_code::logic_error, "user already exists in graph");
        }

        std::weak_ptr<social_manager> thisWeakPtr = shared_from_this();

#if UNIT_TEST_SERVICES
        auto newGraph = std::shared_ptr<MockSocialGraph>(
            new MockSocialGraph(
#else
        auto newGraph = std::shared_ptr<social_graph>(
            new social_graph(
#endif
            user,
            extraDetailLevel,
            [thisWeakPtr, user]()
            {
                std::shared_ptr<social_manager> pThis(thisWeakPtr.lock());
                if (pThis)
                {
                    std::lock_guard<std::mutex> eventLock(pThis->m_socialManagerEventLock);
                    social_event userRemovedEvent(
                        user,
                        social_event_type::local_user_removed,
                        std::vector<xbox_user_id_container>()
                        );
                    pThis->m_eventQueue.push_back(userRemovedEvent);
                }
            }
            ));

        newGraph->initialize()
        .then([thisWeakPtr, user, userString](xbox_live_result<void> result)
        {
            std::shared_ptr<social_manager> pThis(thisWeakPtr.lock());
            if (pThis)
            {
                if (result.err())
                {
                    std::lock_guard<std::mutex> eventLock(pThis->m_socialManagerEventLock);
                    pThis->m_eventQueue.push_back(
                        social_event(
                            user,
                            social_event_type::local_user_added,
                            std::vector<xbox_user_id_container>(),
                            std::make_shared<social_event_args>(),
                            result.err(),
                            result.err_message()
                            )
                        );
                }
                else
                {
                    std::lock_guard<std::mutex> lock(pThis->m_socialMangerLock);

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
                                    std::weak_ptr<social_manager> socialManagerWeakPtr = pThis;
                                    currentView->initialize_filter_list(
                                        *pThis->m_localGraphs[userString]->active_buffer_social_graph()
                                        );

                                    std::lock_guard<std::mutex> eventLock(pThis->m_socialManagerEventLock);
                                    pThis->m_eventQueue.push_back(
                                        social_event(
                                            user,
                                            social_event_type::social_user_group_loaded,
                                            std::vector<xbox_user_id_container>(),
                                            std::make_shared<social_user_group_loaded_event_args>(currentView)
                                            )
                                        );
                                }
                            }

                        }
                    }
                    
                    std::lock_guard<std::mutex> eventLock(pThis->m_socialManagerEventLock);
                    pThis->m_eventQueue.push_back(
                        social_event(
                            user,
                            social_event_type::local_user_added,
                            std::vector<xbox_user_id_container>()
                            )
                        );
                }
            }
        });

        m_localGraphs[userString] = newGraph;
    }

    {
        std::lock_guard<std::mutex> lock(m_socialMangerLock);
        m_localUserList.push_back(user);
    }
    return xbox_live_result<void>();
}

xbox_live_result<void>
social_manager::remove_local_user(
    _In_ xbox_live_user_t user
    )
{
    if (user == nullptr)
    {
        return xbox_live_result<void>(xbox_live_error_code::invalid_argument, "User cannot be null");
    }

    std::lock_guard<std::mutex> lock(m_socialMangerLock);

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

    std::vector<xbox_live_user_t> userList;
    uint32_t i;
    for (i = 0; i < m_localUserList.size(); ++i)
    {
        auto localUser = m_localUserList[i];
        string_t userId = user_context::get_user_id(user);
        string_t localUserId = user_context::get_user_id(localUser);
        if (utils::str_icmp(localUserId, userId) != 0)
        {
            userList.push_back(localUser);
        }
    }

    m_localUserList = userList;
    return xbox_live_result<void>();
}

std::vector<social_event>
social_manager::do_work()
{
    std::lock_guard<std::mutex> lock(m_socialMangerLock);
    std::lock_guard<std::mutex> eventLock(m_socialManagerEventLock);
    std::vector<social_event> socialEvents(m_eventQueue);
    auto xsapiSingleton = get_xsapi_singleton();
    xsapiSingleton->m_perfTester->start_timer(_T("do_work"));
    xsapiSingleton->m_perfTester->start_timer(_T("do_work: eventqueue clear"));
    m_eventQueue.clear();
    xsapiSingleton->m_perfTester->stop_timer(_T("do_work: eventqueue clear"));
    for (auto& graph : m_localGraphs)
    {
        xsapiSingleton->m_perfTester->start_timer(_T("do_work: social_graph do_work"));
        auto graphData = graph.second->do_work(socialEvents);
        xsapiSingleton->m_perfTester->stop_timer(_T("do_work: social_graph do_work"));
        const auto& userViewList = m_userToViewMap[graph.first];
        for (auto& viewHash : userViewList)
        {
            auto& view = m_xboxSocialUserGroups[viewHash];
            if(graphData.socialUsers != nullptr)
            {
                xsapiSingleton->m_perfTester->start_timer(_T("do_work: update_view"));
                view->update_view(*graphData.socialUsers, socialEvents);
                xsapiSingleton->m_perfTester->stop_timer(_T("do_work: update_view"));
            }
        }
    }

    xsapiSingleton->m_perfTester->stop_timer(_T("do_work"));
    xsapiSingleton->m_perfTester->clear();
    return socialEvents;
}

const std::vector<xbox_live_user_t>&
social_manager::local_users() const
{
    return m_localUserList;
}

xbox_live_result<void> social_manager::update_social_user_group(
    _In_ const std::shared_ptr<xbox_social_user_group>& socialGroup,
    _In_ const std::vector<string_t>& users
    )
{
    std::lock_guard<std::mutex> lock(m_socialMangerLock);

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

    auto diffUsers = socialGroup->_Update_users_in_group(users);

    auto localUser = socialGroup->local_user();
    string_t xuid = user_context::get_user_id(localUser);
    m_localGraphs[xuid]->remove_users(utils::xsapi_vector_to_std_vector(diffUsers.removeGroup));
    std::weak_ptr<social_manager> thisWeakPtr = shared_from_this();
    
    auto hash = socialGroup->hash();
    pplx::task_completion_event<xbox_live_result<void>> tce;
    m_localGraphs[xuid]->add_users(utils::xsapi_vector_to_std_vector(diffUsers.addGroup), tce);
    create_task(tce).then([thisWeakPtr, localUser, users, hash](xbox_live_result<void> userResult)
    {
        std::shared_ptr<social_manager> pThis(thisWeakPtr.lock());
        if(pThis)
        {
            std::lock_guard<std::mutex> eventLock(pThis->m_socialManagerEventLock);

            // send event
            pThis->m_eventQueue.push_back(
                social_event(
                    localUser,
                    social_event_type::social_user_group_updated,
                    std::vector<xbox_user_id_container>(),
                    nullptr,
                    userResult.err(),
                    userResult.err_message()
                    )
            );
        }
    });

    return xbox_live_result<void>();
}

xbox_live_result<void>
social_manager::set_rich_presence_polling_status(
    _In_ xbox_live_user_t user,
    _In_ bool shouldEnablePolling
    )
{
    std::lock_guard<std::mutex> lock(m_socialMangerLock);
    auto localGraphIter = m_localGraphs.find(user_context::get_user_id(user));
    if (localGraphIter == m_localGraphs.end())
    {
        return xbox_live_result<void>(xbox_live_error_code::logic_error, "User not found in graph");
    }

    localGraphIter->second->enable_rich_presence_polling(shouldEnablePolling);
    return xbox_live_result<void>();
}

void social_manager::_Log_state()
{
    LOGS_DEBUG << "[SM] State: m_xboxSocialUserGroups: " << m_xboxSocialUserGroups.size()
        << " m_userToViewMap: " << m_userToViewMap.size()
        << " m_localGraphs: " << m_localGraphs.size()
        << " m_eventQueue: " << m_eventQueue.size()
        << " m_localUserList: " << m_localUserList.size();
}

NAMESPACE_MICROSOFT_XBOX_SERVICES_SOCIAL_MANAGER_CPP_END
